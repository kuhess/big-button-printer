import click
import more_itertools as mit
from PIL import Image, ImageOps
from PIL.Image import FLOYDSTEINBERG
from jinja2.nativetypes import NativeEnvironment

TEMPLATE_STR = """uint8_t bitmap[] = {
    // {{ width }}x{{ height }}px
    {% set line_sep = joiner(",\\n    ") %}
    {%- for chunk in hex_values_chunks -%}
    {{ line_sep() }}
    {%- set inline_sep = joiner(", ") -%}
    {%- for hex_value in chunk -%}
    {{ inline_sep() }}{{ hex_value }}
    {%- endfor -%}
    {% endfor %}
};
"""


@click.command()
@click.option("-w", "--width", default=384)
@click.argument("input_file")
def generate_cpp_code(width, input_file):
    with Image.open(input_file) as im:
        resized_im = ImageOps.contain(im, (width, 10000))
        bw_im = resized_im.convert(mode="1", dither=FLOYDSTEINBERG)

        # invert pixels
        l_im = bw_im.convert('L')
        inv_l_im = ImageOps.invert(l_im)
        inverted_im = inv_l_im.convert('1')

        # create CPP variable declaration
        data = inverted_im.tobytes()

        hex_values = ["0x%02x" % d for d in data]
        hex_values_chunks = mit.chunked(hex_values, 16)

        env = NativeEnvironment()
        template = env.from_string(TEMPLATE_STR)
        result = template.render(
            width=inverted_im.width,
            height=inverted_im.height,
            hex_values_chunks=hex_values_chunks
        )

        print(result)


if __name__ == "__main__":
    generate_cpp_code()
