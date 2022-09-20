import click
from PIL import Image, ImageOps
from PIL.Image import FLOYDSTEINBERG
from typing import Iterator


def ceiling_division(n, d):
    return -(n // -d)


def get_slices(image: Image) -> Image:
    slice_height = 24
    width, height = image.size

    num_slices = ceiling_division(height, slice_height)

    for i in range(num_slices):
        box = (0, i * slice_height, width, (i + 1) * slice_height)
        slice = image.crop(box)
        yield slice


def slice_to_bytes(slice: Image) -> bytes:
    width, _ = slice.size

    rows_bytes = []
    for row in range(3):
        box = (0, row * 8, width, (row + 1) * 8)
        row_bytes = (
            slice.crop(box)
            .transpose(Image.ROTATE_270)
            .transpose(Image.FLIP_LEFT_RIGHT)
            .tobytes()
        )
        rows_bytes.append(row_bytes)

    all_bytes = bytes()
    for i in range(width):
        all_bytes += (rows_bytes[0][i]).to_bytes(1, byteorder="little")
        all_bytes += (rows_bytes[1][i]).to_bytes(1, byteorder="little")
        all_bytes += (rows_bytes[2][i]).to_bytes(1, byteorder="little")

    return all_bytes


def resize_and_dither(image: Image, width: int):
    resized_im = ImageOps.contain(image, (width, 10000))  # TODO magic number
    return resized_im.convert(mode="1", dither=FLOYDSTEINBERG)


def convert_to_printer_raw(image_bw: Image) -> Iterator[bytes]:
    # invert pixels
    l_im = image_bw.convert("L")
    inv_l_im = ImageOps.invert(l_im)
    inverted_im = inv_l_im.convert("1")

    for slice in get_slices(inverted_im):
        yield slice_to_bytes(slice)


@click.command()
@click.option("-w", "--width", default=384)
@click.argument("input_file")
@click.argument("output_file")
def convert(width, input_file, output_file):
    with Image.open(input_file) as im:
        bw_im = resize_and_dither(im, width)

        if output_file.endswith(".raw"):
            # save as binary format
            with open(output_file, "wb") as out:
                for data in convert_to_printer_raw(bw_im):
                    out.write(data)
        else:
            # save as image
            bw_im.save(output_file)


if __name__ == "__main__":
    convert()
