#include <iostream>
#include <fstream>
#include <Windows.h>
#include <shobjidl.h>
char Gradient[] = " .:-=+*/0#%@";
void MakeArt(PWSTR path) {
    HANDLE picture = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    DWORD readWord;
    std::ofstream res("File.txt");
    if (picture == INVALID_HANDLE_VALUE) return;
    ReadFile(picture, &bfh, sizeof(bfh), &readWord, NULL);
    ReadFile(picture, &bih, sizeof(bih), &readWord, NULL);
    std::cout<< "Width: " << bih.biWidth << " Height:" << bih.biHeight << std::endl;
    OVERLAPPED overlapped{ 0 };
    LARGE_INTEGER li; li.QuadPart = 54 + 28 * 3;

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
            color-=60;
            color *= 2;
            color = (color < 255)? color : 255;
            
            //result symb
            result [x][y]=Gradient[color*11/256];
            li.QuadPart += 3;
        }
        
        li.QuadPart+= bih.biWidth%4; // ignore extra byte
    
    }
    for (int y = bih.biHeight-1; y >=0; y--) {
        for (int x = bih.biWidth-1; x >=0; x--)
        {
            //output result
            res << result[x][y];

        }

        res << '\n';
    }
    res.close();

    
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
