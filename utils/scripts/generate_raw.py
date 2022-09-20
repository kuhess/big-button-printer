import click
from PIL import Image, ImageOps
from PIL.Image import FLOYDSTEINBERG


def ceiling_division(n, d):
    return -(n // -d)

def get_slices(image: Image) -> Image:
    slice_height = 24
    width, height = image.size

    num_slices = ceiling_division(height, slice_height)

    for i in range(num_slices):
        box = (0, i*slice_height, width, (i+1)*slice_height)
        slice = image.crop(box)
        yield slice

def slice_to_bytes(slice: Image) -> bytes:
    width, _ = slice.size

    rows_bytes = []
    for row in range(3):
        box = (0, row*8, width, (row+1)*8)
        row_bytes = slice.crop(box).transpose(Image.ROTATE_270).transpose(Image.FLIP_LEFT_RIGHT).tobytes()
        rows_bytes.append(row_bytes)

    all_bytes = bytes()
    for i in range(width):
        all_bytes += (rows_bytes[0][i]).to_bytes(1, byteorder="little")
        all_bytes += (rows_bytes[1][i]).to_bytes(1, byteorder="little")
        all_bytes += (rows_bytes[2][i]).to_bytes(1, byteorder="little")

    return all_bytes

@click.command()
@click.option("-w", "--width", default=384)
@click.argument("input_file")
@click.argument("output_file")
def generate_raw(width, input_file, output_file):
    with Image.open(input_file) as im:
        resized_im = ImageOps.contain(im, (width, 10000))
        bw_im = resized_im.convert(mode="1", dither=FLOYDSTEINBERG)

        # invert pixels
        l_im = bw_im.convert('L')
        inv_l_im = ImageOps.invert(l_im)
        inverted_im = inv_l_im.convert('1')

        with open(output_file, "wb") as out:
            for slice in get_slices(inverted_im):
                data = slice_to_bytes(slice)
                out.write(data)


def run_sudokus():
    for i in range(10000):
        input_path = f"images/sudoku/sudoku_{i:04d}.png"
        output_path = f"images/output/sudoku/sudoku_{i:04d}.raw"
        generate_raw(384, input_path, output_path)


if __name__ == "__main__":
    generate_raw()
