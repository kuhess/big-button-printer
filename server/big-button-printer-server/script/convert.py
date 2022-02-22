import click
from PIL import Image, ImageOps
from PIL.Image import FLOYDSTEINBERG


def lighten(image: Image, factor: float) -> Image:
    degenerate = Image.new(image.mode, image.size, (255, 255, 255))
    return Image.blend(degenerate, image, factor)


def convert_bw(image: Image, width: int, lighten_factor: int) -> Image:
    resized_im = ImageOps.contain(image, (width, 10000))
    light_im = lighten(resized_im, lighten_factor)
    bw_im = light_im.convert(mode="1", dither=FLOYDSTEINBERG)
    return bw_im


def invert_bw(image: Image) -> Image:
    l_im = image.convert('L')
    inv_l_im = ImageOps.invert(l_im)
    return inv_l_im.convert('1')


@click.command()
@click.option("-w", "--width", default=384)
@click.option("-l", "--lighten-factor", default=0.5)
@click.argument("input_file")
@click.argument("output_file")
def convert_bw_and_save(width, lighten_factor, input_file, output_file):
    with Image.open(input_file) as im:
        bw_im = convert_bw(im, width, lighten_factor)
        if output_file.endswith(".raw"):
            inverted_im = invert_bw(bw_im)
            # save as raw bytes
            data = inverted_im.tobytes()
            with open(output_file, "wb") as f:
                f.write(data)
        else:
            # save as img
            bw_im.save(output_file)


if __name__ == "__main__":
    convert_bw_and_save()
