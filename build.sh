#!/bin/bash

# Проверяем наличие необходимых компонентов
if ! command -v g++ &> /dev/null; then
    echo "g++ не установлен. Установка..."
    sudo apt-get update
    sudo apt-get install -y g++
fi

# Устанавливаем необходимые библиотеки
if ! dpkg -l | grep -q libcurl4-openssl-dev; then
    echo "libcurl не установлен. Установка..."
    sudo apt-get install -y libcurl4-openssl-dev
fi

if ! dpkg -l | grep -q libjsoncpp-dev; then
    echo "libjsoncpp не установлен. Установка..."
    sudo apt-get install -y libjsoncpp-dev
fi

# Компилируем программу
echo "Компиляция программы..."
g++ -std=c++11 main.cpp -o youtube_downloader -lcurl -ljsoncpp

if [ $? -eq 0 ]; then
    echo "Компиляция успешно завершена!"
    echo "Теперь вы можете использовать программу: ./youtube_downloader <URL>"
else
    echo "Ошибка при компиляции!"
fi