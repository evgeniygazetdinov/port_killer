#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Использование: ./run.sh <YouTube URL>"
    exit 1
fi

# Проверяем, существует ли исполняемый файл
if [ ! -f "./youtube_downloader" ]; then
    echo "Программа не скомпилирована. Запуск сборки..."
    ./build.sh
fi

# Запускаем программу
./youtube_downloader "$1"