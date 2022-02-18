import click
from PIL import Image, ImageOps
from PIL.Image import FLOYDSTEINBERG


@click.command()
@click.option("-w", "--width", default=384)
@click.argument("input_file")
@click.argument("output_file")
def convert_bw(width, input_file, output_file):
    with Image.open(input_file) as im:
        resized_im = ImageOps.contain(im, (width, 10000))
        bw_im = resized_im.convert(mode="1", dither=FLOYDSTEINBERG)

        if output_file.endswith(".raw"):
            # invert pixels
            l_im = bw_im.convert('L')
            inv_l_im = ImageOps.invert(l_im)
            inverted_im = inv_l_im.convert('1')
            # save as bin
            data = inverted_im.tobytes()
            with open(output_file, "wb") as f:
                f.write(data)
        else:
            # save as img
            bw_im.save(output_file)


@click.command()
@click.argument("input_file")
@click.argument("output_file")
def bin_to_img(width, input_file, output_file):
    with Image.open(input_file) as im:
        resized_im = ImageOps.contain(im, (width, 10000))
        bw_im = resized_im.convert(mode="1", dither=FLOYDSTEINBERG)

        if output_file.endswith(".bin"):
            # invert pixels
            l_im = bw_im.convert('L')
            inv_l_im = ImageOps.invert(l_im)
            inverted_im = inv_l_im.convert('1')
            # save as bin
            data = inverted_im.tobytes()
            with open(output_file, "wb") as f:
                f.write(data)
        else:
            # save as img
            bw_im.save(output_file)


if __name__ == "__main__":
    convert_bw()


if __name__ == "__main__":
    convert_bw()
