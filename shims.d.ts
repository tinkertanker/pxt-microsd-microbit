// Auto-generated. Do not edit.



    //% color=70 weight=80
declare namespace SD {

    /**
     * more secret stuff!
     */
    //% blockId=sd_open_filehandle
    //% block="open a filehandle for %file for reading? %read" shim=SD::fileHandle
    function fileHandle(file: string, read: boolean): void;

    /**
     * secret stuff!
     */
    //% blockId=sd_read_sixteen
    //% block="16 bytes from the filehandle" shim=SD::readAligned
    function readAligned(): string;

    /**
     *more secret stuffffff
     */
    //% blockId=sd_read_byte
    //% block="1 byte from filehandle" shim=SD::readSingleByte
    function readSingleByte(): number;

    /**
     * final secret item
     */
    //% blockId=sd_close_filehandle
    //% block="close filehandle" shim=SD::closeFile
    function closeFile(): void;

    /**
     * i guess it's not so secret
     */
    //% blockId=sd_write_filehandle
    //% block="write %str| to filehandle" shim=SD::writeFileHandle
    function writeFileHandle(str: string): void;

    /**
     * reads a file from the sd card.
     */
    //% blockId=sd_read_file
    //% block="the contents of |%file" shim=SD::readFile
    function readFile(file: string): string;

    /**
     * creates a file on the sd card.
     */
    //% blockId=sd_create_file
    //% block="create a file called |%file" shim=SD::createFile
    function createFile(file: string): void;

    /**
     * creates a folder on the sd card.
     */
    //% blockId=sd_create_folder
    //% block="create a folder called |%folder" shim=SD::createFolder
    function createFolder(file: string): void;

    /**
     * checks if a file exists (will return false on folder as well)
     */
    //% blockId=sd_file_exists
    //% block="the file %file| exists" shim=SD::fileExists
    function fileExists(file: string): boolean;

    /**
     * checks if a folder exists
     */
    //% blockId=sd_folder_exists
    //% block="the folder %folder| exists" shim=SD::folderExists
    function folderExists(folder: string): boolean;

    /**
     * returns the amount of stuff in a folder
     */
    //% blockId=sd_lscount
    //% block="the number of items in folder %folder|" shim=SD::contents
    function contents(folder: string): number;

    /**
     * gets the i-th item in a folder
     */
    //% blockId=sd_ls
    //% block="the %count|th item in %folder|" shim=SD::getItem
    function getItem(count: number, folder: string): string;

    /**
     * writes a string to a file.
     */
    //% blockId=sd_write_file
    //% block="write %stuff| to %file|" shim=SD::writeFile
    function writeFile(stuff: string, file: string): void;

    /**
     * initialises the sd card.
     */
    //% blockId=sd_init
    //% block="connected SD to |%pin" shim=SD::connect
    function connect(pin: Pins): void;
}

// Auto-generated. Do not edit. Really.
