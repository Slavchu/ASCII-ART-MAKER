#include <iostream>
#include <cmath>
#include <fstream>
#include <Windows.h>
#include <shobjidl.h>
char Gradient[] = " .:-=+*/0#%";
void MakeArt(PWSTR path) {
    HANDLE picture = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    int contNum; float conMul;
    std::cout << "Enter contrast value(0-255):";
    std::cin >> contNum;
    std::cout << "Enter contrast multiplyer(-255 - 255): ";
    std::cin >> conMul;
    DWORD readWord;
    
    if (picture == INVALID_HANDLE_VALUE) return;
    ReadFile(picture, &bfh, sizeof(bfh), &readWord, NULL);
    ReadFile(picture, &bih, sizeof(bih), &readWord, NULL);
    std::cout<< "Width: " << bih.biWidth << " Height:" << bih.biHeight << std::endl;
    if (bih.biBitCount != 24) {
        
        std::cout << "Invalid file format\n";
        system("pause");
        return; 
    }
    OVERLAPPED overlapped{ 0 };
    LARGE_INTEGER li; li.QuadPart = 54;
    std::ofstream res("God Given Girl.txt");
    char** result = new char * [bih.biWidth];
    for (int x = 0; x < bih.biWidth; x++) {
        result[x] = new char[bih.biHeight];
    }
    unsigned char pixel[4];
    for (int y = 0; y < bih.biHeight; y++) {
        for (int x = 0; x < bih.biWidth; x++)
        {
                    
            overlapped.Offset = li.LowPart;
            overlapped.OffsetHigh = li.HighPart;
            ReadFile(picture, &pixel, 3, &readWord, &overlapped); //reading file
            int color = pixel[0] + pixel[1] + pixel[2]; //black & white converation
            color /= 3;
            
            
            // Add contrast
            color-=contNum;
            color *= conMul;
            color = (color < 255)? color : 255;
           color = (color < 0) ? 0 : color;
           //color = 255 - color;
           short index = std::round((double)(color * 10 / 255));
           
            result [x][y]=Gradient[index];
            li.QuadPart += 3;
        }
        
       li.QuadPart+=bih.biWidth%4; // ignore extra byte
    
    }
    for (int y = bih.biHeight-1; y >=0; y--) {
        for (int x =0; x <bih.biWidth; x++)
        {
            //output result
            res << result[x][y];// result[x][y];

        }

        res << '\n';
    }
    res.close();
    
    //free memory
   for (int x = 0; x < bih.biWidth; x++) {     
       delete[]result[x];
   }
   delete[]result;
    
}
int main()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            COMDLG_FILTERSPEC spec[]{
                {L"BMP PHOTO", L"*.bmp"}
            };
            pFileOpen->SetFileTypes(1, spec);
    
            hr = pFileOpen->Show(NULL);

            
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    
                    MakeArt(pszFilePath);
                    if (SUCCEEDED(hr))
                    {
                        
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
}
