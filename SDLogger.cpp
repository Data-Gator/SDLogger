#include "SDLogger.h"

/**
 * Constructor to set the file name
 */
SDLogger::SDLogger(std::string filename){
    this->filename = filename + filetype;
    
    this->initialize_sd_card();
}


SDLogger::SDLogger(std::string prefix, int month, int day, int year, std::string filetype){
    this->filename = prefix + 
        "_" + std::to_string(month) + 
        "-" + std::to_string(day) +
        "-" + std::to_string(year) + 
        filetype;
}


bool SDLogger::initialize_sd_card(){

    const auto ok = sd.begin(TT_CLK, TT_MISO, TT_MOSI, TT_SS, &SPI);

    if(!ok){
        throw -1;
        Serial.println("ERROR initializing sd card");
        return false;
    }

    return true;
}

/**
 * Opens the file attached to the logger object with the default
 *  mode
 */
File SDLogger::open_file(const char* mode){
    try{
        return sd.open(filename.c_str(), mode);

    }catch(const std::exception& e){
        Serial.println(e.what());
     
    }catch(...){
        Serial.println("ERROR opening file");
    }

}

/**
 * Close the SD card connection/file 
 */
void SDLogger::close_file(){
    this->sd.end();
}

/**
 * Write a line to the file, will overwrite the 
 *  contents of the file
 */
void SDLogger::write_line(std::string line){
    File f = this->open_file(FILE_WRITE);

    f.write('\n');

    const char* buf = line.c_str();
    f.write((uint8_t*)buf, line.length());
}

/**
 * Open file and append a line, add newline
 *  if needed
 */
void SDLogger::append_line(std::string line){
    File f = this->open_file(FILE_APPEND);

    f.write('\n');

    const char* buf = line.c_str();
    f.write((uint8_t*)buf, line.length());
}


/**
 * Append a line to the target csv file containing 
 *  an absolute time stamp, mqtt topic, and an mqtt message
 */
void SDLogger::log_absolute_mqtt(std::string time, std::string mqtt_topic, std::string mqtt_message){
    std::string line = time + separator + 
        mqtt_topic + separator + 
        mqtt_message + separator;

    this->append_line(line);
}

/**
 * Append a line with a relative time stamp to the 
 *  target file
 */
void SDLogger::log_relative_mqtt(std::string time, int offset, std::string mqtt_topic, std::string mqtt_message){
    time = time + "+" + std::to_string(offset);

    this->log_absolute_mqtt(time, mqtt_topic, mqtt_message);
}


/**
 * Writes a header line to a file using append to 
 *  prevent overwriting valuable data.
 */
void SDLogger::write_header(std::vector<std::string> fields){
    std::string header = "";
    for(std::string i : fields){
        header += i + separator;
    }

    this->append_line(header);
}