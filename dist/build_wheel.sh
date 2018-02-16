# copy the library to the distribution package folder
if [[ "$OSTYPE" == "linux-gnu" ]]; then
  cp ../cmake-build-debug/libTFMongoDB.so tfmongodb/
  cp ../src/python/tfmongodb_linux.py tfmongodb/__init__.py
elif [[ "$OSTYPE" == "darwin"* ]]; then
  cp ../cmake-build-debug/libTFMongoDB.dylib tfmongodb/
  cp ../src/python/tfmongodb_macosx.py tfmongodb/__init__.py
else
  echo "Your system is currently not supported"
fi

# let setup.py build the wheel
python setup.py bdist_wheel
