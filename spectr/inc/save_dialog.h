/**
 * @file save_dialog.h
 * @author Sergey Simonov (sb.simonov@gmail.com)
 */

#include <string>
#include <windows.h>
#include <iostream>

/**
 * @brief Запускает стандартного Windows окна запроса файла для сохранения
 * @return std::wstring - путь к файлу, или пустая строка, если произошла ошибка/отмена 
 */
std::wstring save_file_dialog() {

    std::wstring ret_str;
    ret_str.resize(2048);
    
    OPENFILENAMEW ofn;
    std::memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize  = sizeof( ofn );
    ofn.hwndOwner    = NULL;  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = L"CSV file\0*.csv\0\0";
    ofn.lpstrDefExt = L"csv";
    ofn.lpstrFile    = ret_str.data();
    ofn.nMaxFile     = static_cast<DWORD>(ret_str.size());
    ofn.lpstrTitle   = L"Сохранить спектр в файл";
    ofn.Flags = OFN_EXPLORER;
    
    bool res = GetSaveFileNameW(&ofn);
    if (res) {    
        return ret_str;
    } else {
        return std::wstring();
    } 
}