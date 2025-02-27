#include <iostream>
#include <string>
#include <curl/curl.h>
#include <regex>
#include <fstream>
#include <sstream>
//#include <json/json.h> // libjsoncpp

class YouTubeDownloader {
private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    static size_t WriteToFile(void* ptr, size_t size, size_t nmemb, FILE* stream) {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    std::string getVideoInfo(const std::string& videoId) {
        CURL* curl = curl_easy_init();
        std::string readBuffer;
        
        if(curl) {
            // Формируем URL для получения информации о видео
            std::string url = "https://www.youtube.com/get_video_info?video_id=" + videoId;
            
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            
            // Устанавливаем User-Agent
            std::string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36";
            curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
            
            CURLcode res = curl_easy_perform(curl);
            
            if(res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }
            
            curl_easy_cleanup(curl);
        }
        
        return readBuffer;
    }

    bool downloadVideoStream(const std::string& url, const std::string& outputPath) {
        CURL* curl = curl_easy_init();
        if(!curl) {
            return false;
        }

        FILE* fp = fopen(outputPath.c_str(), "wb");
        if(!fp) {
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        
        // Добавляем заголовки
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: video/webm,video/ogg,video/*;q=0.9,application/ogg;q=0.7,audio/*;q=0.6,*/*;q=0.5");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Устанавливаем User-Agent
        std::string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36";
        curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());

        // Устанавливаем таймаут
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600L); // 10 минут

        // Включаем progress meter
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        CURLcode res = curl_easy_perform(curl);

        curl_slist_free_all(headers);
        fclose(fp);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            std::cerr << "Ошибка загрузки: " << curl_easy_strerror(res) << std::endl;
            remove(outputPath.c_str());
            return false;
        }

        return true;
    }

public:
    bool downloadVideo(const std::string& url, const std::string& outputPath) {
        // Извлекаем ID видео из URL
        std::regex regex("(?:v=|\\/)([a-zA-Z0-9_-]{11})");
        std::smatch match;
        if (!std::regex_search(url, match, regex)) {
            std::cerr << "Неверный формат URL\n";
            return false;
        }
        std::string videoId = match[1];

        // Получаем информацию о видео
        std::string videoInfo = getVideoInfo(videoId);
        if (videoInfo.empty()) {
            std::cerr << "Не удалось получить информацию о видео\n";
            return false;
        }

        // Пытаемся загрузить видео напрямую
        std::string directUrl = "https://www.youtube.com/watch?v=" + videoId;
        return downloadVideoStream(directUrl, outputPath);
    }

    static bool isValidUrl(const std::string& url) {
        std::regex youtube_regex(
            "^(https?:\\/\\/)?(www\\.)?"
            "(youtube\\.com\\/watch\\?v=|youtu\\.be\\/)"
            "[a-zA-Z0-9_-]{11}$"
        );
        return std::regex_match(url, youtube_regex);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Использование: " << argv[0] << " <YouTube URL>\n";
        return 1;
    }

    std::string url = argv[1];
    if (!YouTubeDownloader::isValidUrl(url)) {
        std::cout << "Ошибка: Неверный формат URL YouTube\n";
        return 1;
    }

    // Инициализируем CURL
    curl_global_init(CURL_GLOBAL_ALL);

    YouTubeDownloader downloader;
    std::string outputPath = "downloaded_video.mp4";
    
    std::cout << "Начинаем загрузку видео...\n";
    bool success = downloader.downloadVideo(url, outputPath);
    
    curl_global_cleanup();

    if (success) {
        std::cout << "Видео успешно загружено в файл: " << outputPath << std::endl;
    } else {
        std::cout << "Произошла ошибка при загрузке видео.\n";
        return 1;
    }

    return 0;
}