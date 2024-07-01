#include <iostream>
namespace swf_utils {
	void padImage(int* input, int* output, const int& rows, const int& cols, const int& radius) {
		// Copy input to output
		for (int row = 0; row < rows; ++row) {
			int pIn = row * cols;

			int outOffset = radius * (cols + 2 * radius) + radius + row * 2 * radius;
			int pOut = row * cols + outOffset;
			memcpy(output + pOut, input + pIn, sizeof(int) * cols);
		}

		// Pad vertically
		for (int row = 0; row < rows + 2 * radius; ++row) {
			if (row == radius) {
				row += rows;
			}

			int pIn = 0;

			if (row >= radius) {
				pIn = (rows - 1) * cols;
			}

			int outOffset = radius + row * 2 * radius;
			int pOut = row * cols + outOffset;

			memcpy(output + pOut, input + pIn, sizeof(int) * cols);
		}

		// Pad horizontally
		for (int row = 0; row < rows + 2 * radius; ++row) {
			for (int col = 0; col < cols + 2 * radius; ++col) {
				if (col == radius) {
					col += cols;
				}

				int iDst = row * (cols + 2 * radius) + col;
				int iSrc = iDst + radius - col;
				if (col >= radius) {
					iSrc += cols - 1;
				}
				output[iDst] = output[iSrc];
			}
		}

	}
}