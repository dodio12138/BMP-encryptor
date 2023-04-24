#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <Windows.h>
#include <math.h>

using namespace std;

#pragma pack(push, 1) // 以字节为单位对齐结构体

struct BMPHeader {
	// BMP文件头结构体
	// 包含文件类型、文件大小、保留字段、图像偏移量等信息
	// 具体结构请参考BMP文件格式文档
	uint16_t bfType = 0x4D42; // BMP文件类型标志，固定为0x4D42（即BM）
	uint32_t bfSize = 0; // BMP文件的大小，以字节为单位，初始化为0
	uint16_t bfReserved1 = 0; // 保留字段，值为0
	uint16_t bfReserved2 = 0; // 保留字段，值为0
	uint32_t bfOffBits = 54; // BMP图像数据相对于文件头的偏移量，以字节为单位
};

struct BMPInfoHeader {
	// BMP信息头结构体
	// 包含图像宽度、高度、颜色位数、压缩方式等信息
	// 具体结构请参考BMP文件格式文档
	uint32_t biSize = sizeof(BMPInfoHeader); // BMPInfoHeader结构体的大小，以字节为单位，初始化为结构体大小
	int32_t biWidth = 0; // BMP图像的宽度，以像素为单位，初始化为0
	int32_t biHeight = 0; // BMP图像的高度，以像素为单位，初始化为0
	uint16_t biPlanes = 1; // BMP图像的颜色平面数，固定为1
	uint16_t biBitCount = 0; // BMP图像每个像素的位数，通常为1、4、8、16、24、32等，初始化为0
	uint32_t biCompression = 0; // BMP图像的压缩类型，通常为0，表示不压缩
	uint32_t biSizeImage = 0; // BMP图像的大小，以字节为单位，初始化为0
	int32_t biXPelsPerMeter = 0; // BMP图像的水平分辨率，以像素/米为单位，初始化为0
	int32_t biYPelsPerMeter = 0; // BMP图像的垂直分辨率，以像素/米为单位，初始化为0
	uint32_t biClrUsed = 0; // BMP图像实际使用的颜色数，如果该值为0，则说明使用所有颜色，初始化为0
	uint32_t biClrImportant = 0; // BMP图像中重要的颜色数，如果该值为0，则说明所有颜色都重要，初始化为0
};
#pragma pack(pop) // 恢复默认的对齐方式

int getPixelSize(int width, int height, int fileSize);

void readBmpPixelData(const string& bmpFilePath, const string& outputFilePath);

string returnFileType(string str);

vector<string> split(const string &s, char delim);

int main() {

	// 设置控制台窗口大小和位置
	HWND console = GetConsoleWindow();
	RECT rect;
	GetWindowRect(console, &rect);
	MoveWindow(console, rect.left, rect.top, 600, 600, TRUE);
	SetWindowPos(console, HWND_TOP, 100, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

	while (1) 
	{
		// 读取输入文件数据
		string input_filename;
		cout << "请输入需要转换的文件:" << endl;
		cin >> input_filename;
		ifstream input_file(input_filename, ios::binary | ios::ate);

		if (input_file)
		{
			cout << "读取成功" << endl;

		}
		else
		{
			cout << "读取失败" << endl;
			return 1;
		}

		string command;
		cout << "选择加密还是解密（E/D）" << endl;
		cin >> command;
		if (command == "E")
		{
			streamsize input_size = input_file.tellg();
			input_file.seekg(0, ios::beg);
			vector<char> input_data(input_size);
			input_file.read(input_data.data(), input_size);

			// 计算BMP文件的大小和数据偏移量
			BMPHeader bmp_header;
			BMPInfoHeader bmp_info_header;
			bmp_header.bfSize = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + input_size; // BMP文件的大小
			bmp_info_header.biWidth = std::sqrt((bmp_header.bfSize - 54) / 4); // BMP图像的宽度，以像素为单位
			bmp_info_header.biHeight = std::sqrt((bmp_header.bfSize - 54) / 4); // BMP图像的高度，以像素为单位
			bmp_info_header.biBitCount = getPixelSize(bmp_info_header.biWidth, bmp_info_header.biHeight, bmp_header.bfSize); // BMP图像每个像素的位数

			// 将BMPHeader和BMPInfoHeader写入输出文件
			// 根据输入文件的大小和BMP文件的头部和像素数据大小计算BMP文件大小
			// 计算BMP文件头部和像素数据的偏移量
			// 构造BMP文件头和信息头
			// 注意：BMP文件头和信息头的各个字段需要按照BMP文件格式要求进行填充

			// 构造BMP文件像素数据
			// 注意：BMP文件像素数据的各个部分需要按照BMP文件格式要求进行填充

			// 将BMP文件数据写入输出文件
			string output_filename = "output.bmp";
			ofstream output_file(output_filename, ios::binary);
			output_file.write(reinterpret_cast<const char*>(&bmp_header), sizeof(bmp_header));
			output_file.write(reinterpret_cast<const char*>(&bmp_info_header), sizeof(bmp_info_header));
			output_file.write(input_data.data(), input_data.size());
			output_file.write(returnFileType(input_filename).c_str(), 6);
			output_file.close();
			cout << "加密成功" << endl;
		}
		else if (command == "D")
		{
			streampos pos = input_file.tellg(); // 获取文件大小
			if (pos >= 6) {
				pos -= 6; // 移动读取位置到文件结尾前6个字节的位置
			}
			else {
				pos = 0;
			}
			input_file.seekg(pos);

			char buffer[6];
			input_file.read(buffer, 6); // 读取倒数6个字节的数据
			std::string str(buffer);
			input_file.seekg(0);
			readBmpPixelData(input_filename, "output." + str);
		}
		else if(command == "exit")
		{
			return 0;
		}
	}

	// 反解码为原始文件
	// 读取BMP文件头和信息头
	// 读取BMP文件像素数据
	// 将BMP文件像素数据保存为原始文件数据

	return 0;
}

void readBmpPixelData(const string& bmpFilePath, const string& outputFilePath)
{
	ifstream bmpFile(bmpFilePath, ios::binary | ios::in);

	if (!bmpFile)
	{
		cerr << "Error: failed to open the BMP file: " << bmpFilePath << endl;
		return;
	}

	// 跳过 BMP 文件头和信息头
	bmpFile.seekg(54, ios::beg);

	// 读取像素数据
	vector<char> pixelData((istreambuf_iterator<char>(bmpFile)), istreambuf_iterator<char>());

	bmpFile.close();

	// 将像素数据写入输出文件
	ofstream outputFile(outputFilePath, ios::binary | ios::out);

	if (!outputFile)
	{
		cerr << "Error: failed to create the output file: " << outputFilePath << endl;
		return;
	}
	else
	{
		cerr << "解码成功"<< endl;
	}

	outputFile.write(pixelData.data(), pixelData.size() - 6);
	outputFile.close();
}

int getPixelSize(int width, int height, int fileSize) {
	int rowSize = ((width * 3) + 3) & (~3); // 每行像素数据的长度（必须是4的倍数）
	int dataSize = rowSize * height; // 图像数据的大小
	int headerSize = sizeof(BMPHeader) + sizeof(BMPInfoHeader); // BMP文件头的大小
	int totalSize = headerSize + dataSize; // BMP文件总大小
	return totalSize == fileSize ? 24 : 32; // 判断BMP图像每个像素的位数（如果总大小等于文件大小，则为24位色；否则为32位色）
}

string returnFileType(string str) 
{
	vector<string> tokens = split(str, '.');
	return tokens[1];
}

vector<string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}