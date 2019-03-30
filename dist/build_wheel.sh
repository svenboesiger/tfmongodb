mkdir -p tfmongodb/

# copy the library to the distribution package folder
if [[ "$OSTYPE" == "linux-gnu" ]]; then
  cp ../cmake-build-debug/libTFMongoDB.so tfmongodb/
  cp ../src/python/tfmongodb.py tfmongodb/__init__.py
  sed -i "s/XXX/libTFMongoDB.so/g" tfmongodb/__init__.py
elif [[ "$OSTYPE" == "darwin"* ]]; then
  cp ../libTFMongoDB.dylib tfmongodb/
  sed 's/XXX/libTFMongoDB.dylib/g' ../src/python/tfmongodb.py > tfmongodb/__init__.py
else
  echo "Your system is currently not supported"
fi

# let setup.py build the wheel
python setup.py bdist_wheel
