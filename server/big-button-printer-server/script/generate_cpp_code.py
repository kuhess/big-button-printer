import click
import more_itertools as mit
from PIL import Image
from jinja2.nativetypes import NativeEnvironment

from script.convert import convert_bw, invert_bw

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
@click.option("-l", "--lighten-factor", default=0.5)
@click.argument("input_file")
def generate_cpp_code(width, lighten_factor, input_file):
    with Image.open(input_file) as im:
        bw_im = convert_bw(im, width, lighten_factor)
        inverted_im = invert_bw(bw_im)

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
