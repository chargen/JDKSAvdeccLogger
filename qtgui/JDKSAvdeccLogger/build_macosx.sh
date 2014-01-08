rm -r -f JDKSAvdeccLogger.app && qmake -spec macx-clang JDKSAvdeccLogger.pro && make && macdeployqt JDKSAvdeccLogger.app -dmg
