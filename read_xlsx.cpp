#include "EasyXLS.h"
#include <conio.h>

int main()
{
    HRESULT hr;

    hr = CoInitialize(0);

    if (SUCCEEDED(hr))
    {
        EasyXLS::IExcelDocumentPtr workbook;
        hr = CoCreateInstance(__uuidof(EasyXLS::ExcelDocument),
                              NULL,
                              CLSCTX_ALL,
                              __uuidof(EasyXLS::IExcelDocument),
                              (void**) &workbook) ;

        if (SUCCEEDED(hr)){

            printf("\nReading file: C:\\Samples\\Tutorial04.xlsx\n");
            if (workbook->easy_LoadXLSXFile("C:\\Samples\\Tutorial04.xlsx"))
            {
                EasyXLS::IExcelWorksheetPtr xlsSecondTab = 
                    (EasyXLS::IExcelWorksheetPtr)workbook->easy_getSheetAt(1);
                EasyXLS::IExcelTablePtr xlsTable = xlsSecondTab->easy_getExcelTable();

                xlsTable->easy_getCell_2("A1")->setValue("Data added by Tutorial36");

                char* cellValue = (char*)malloc(11*sizeof(char));
                char* columnNumber = (char*)malloc(sizeof(char));
                for (int column=0; column<5; column++)
                {
                    strcpy_s(cellValue, 6, "Data ");
                    _itoa_s(column+ 1, columnNumber, 2, 10);
                    strcat_s(cellValue, 10, columnNumber);
                    xlsTable->easy_getCell(1, column)->setValue(cellValue);
                }

                printf("Writing file C:\\Samples\\Tutorial36 - read XLSX file.xlsx.");
                workbook->easy_WriteXLSXFile("C:\\Samples\\Tutorial36 - read XLSX file.xlsx");

                _bstr_t sError = workbook->easy_getError();
                if (strcmp(sError, "") == 0){
                    printf("\nFile successfully created. Press Enter to Exit...");
                }
                else{
                    printf("\nError encountered: %s", (LPCSTR)sError);
                }
            }
            else
            {
                printf("\nError reading file C:\\Samples\\Tutorial04.xlsx %s\n",
                            (LPCSTR)((_bstr_t)workbook->easy_getError()));
            }

            // Dispose memory
            workbook->Dispose();
        }
        else{
            printf("Object is not available!");
        }
    }
    else{
        printf("COM can't be initialized!");
    }

    // Uninitialize COM
    CoUninitialize();

    _getch();
    return 0;
}