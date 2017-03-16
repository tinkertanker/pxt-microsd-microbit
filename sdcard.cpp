// Documentation on defining PXT blocks:
// https://www.pxt.io/defining-blocks

#include "pxt.h"
#include "SDFileSystem.h"
using namespace pxt;


SDFileSystem *sd;
char buf[50];
struct stat buffer; 
FILE *f;
DIR *dir;
struct dirent *ent;
enum class Pins{
P0=  3,
P1=  2,
P2=  1,
P3=  4,
P4=  5,
P5=  17,
P6=  12,
P7=  11,
P8=  18,
P9=  10,
P10= 6,
P11= 26,
P12= 20,
P13= 23,
P14= 22,
P15= 21,
P16= 16,
P19= 0,
P20= 30};
//% color=70 weight=80
namespace SD {

  /**
  * more secret stuff!
  */
  //% blockId=sd_open_filehandle
  //% block="open a filehandle for %file| for reading? %read|"
  void fileHandle(StringData *file, bool read){
    sprintf(buf, "/sd/%s", file->data);
    if(read) f = fopen(buf, "r");
    else f= fopen(buf, "a");
  }

  /**
  * secret stuff!
  */
  //% blockId=sd_read_sixteen
  //% block="16 bytes from the filehandle"
  StringData *readAligned(){
    memset(buf, 0, sizeof buf);
    fread(buf, sizeof(unsigned char), 16, f);
    return ManagedString(buf).leakData();
  }

  /**
  *more secret stuffffff
  */
  //% blockId=sd_read_byte
  //% block="1 byte from filehandle"
  int readSingleByte(){
    fread(buf, sizeof(char), 1, f);
    return (int)((256 + buf[0]) % 256);
  }

  /**
  * final secret item
  */
  //% blockId=sd_close_filehandle
  //% block="close filehandle"
  void closeFile(){
    fclose(f);
    free(f);
  }

  /**
  * i guess it's not so secret
  */
  //% blockId=sd_write_filehandle
  //% block="write %str| to filehandle"
  void writeFileHandle(StringData *str){
    fwrite(str->data, sizeof(char), strlen(str->data), f);
  }

  /**
  * reads a file from the sd card.
  */
  //% blockId=sd_read_file
  //% block="the contents of |%file"
  StringData *readFile(StringData *file){
    sprintf(buf, "/sd/%s", file->data);
    f = fopen(buf, "r");
    fseek(f, 0, SEEK_END);
    int fsize = ftell(f);
    rewind(f);
    memset(buf, 0, sizeof buf);
    fread(buf, sizeof(char), min(fsize, 50), f);
    fclose(f);
    free(f);
    return ManagedString(buf).leakData();
  }

  /**
  * creates a file on the sd card.
  */
  //% blockId=sd_create_file
  //% block="create a file called |%file"
  void createFile(StringData *file){
    sprintf(buf, "/sd/%s", file->data);
    f = fopen(buf, "w");
    fclose(f);
    free(f);
  }

  /**
  * creates a folder on the sd card.
  */
  //% blockId=sd_create_folder
  //% block="create a folder called |%folder"
  void createFolder(StringData *file){
    sprintf(buf, "/sd/%s", file->data);
    mkdir(buf, 0777);
  }


  /**
  * checks if a file exists (will return false on folder as well)
  */
  //% blockId=sd_file_exists
  //% block="the file %file| exists"
  bool fileExists(StringData *file){
    sprintf(buf, "/sd/%s", file->data);
    if(stat (buf, &buffer) == 0) return true;
    return false;
  }

  /**
  * checks if a folder exists
  */
  //% blockId=sd_folder_exists
  //% block="the folder %folder| exists"
  bool folderExists(StringData *folder){
    sprintf(buf, "/sd/%s", folder->data);
    dir = opendir(buf);
    if(dir == NULL) return false;
    closedir(dir);
    return true;
  }

  /**
  * returns the amount of stuff in a folder
  */
  //% blockId=sd_lscount
  //% block="the number of items in folder %folder|"
  int contents(StringData *folder){
    int ret = 0;
    sprintf(buf, "/sd/%s", folder->data);
    dir = opendir(buf);
    if(dir == NULL) return ret;
    while(ent = readdir(dir)) ret++;
    closedir(dir);
    return ret;
  }

  /**
  * gets the i-th item in a folder
  */
  //% blockId=sd_ls
  //% block="the %count|th item in %folder|"
  StringData *getItem(int count, StringData *folder){
    sprintf(buf, "/sd/%s", folder->data);
    dir = opendir(buf);
    if(dir == NULL) return ManagedString("failed").leakData();
    struct dirent *ent;
    for(int i=0;i<count;i++){
      ent = readdir(dir);
      if(ent == NULL) break;
    }
    if(ent == NULL) {closedir(dir); return ManagedString("failed").leakData();}
    else {closedir(dir); return ManagedString(ent->d_name).leakData();}
  }

  /**
  * writes a string to a file.
  */
  //% blockId=sd_write_file
  //% block="write %stuff| to %file|"
  void writeFile(StringData *stuff, StringData *file){
    sprintf(buf, "/sd/%s", file->data);
    f = fopen(buf, "a");
    fwrite(stuff->data, sizeof(char), strlen(stuff->data), f);
    fclose(f);
    free(f);
  }

  /**
  * initialises the sd card.
  */
  //% blockId=sd_init
  //% block="connected SD to |%pin"
  void connect(Pins pin){
    printf("%i != %i?\r\n", pin, (PinName)pin);
    sd = new SDFileSystem(MOSI, MISO, SCK, (PinName)pin, "sd");
  }
}