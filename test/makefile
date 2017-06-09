all: start-index line-numbers-file

start-index:
	@echo "Testing use of start index and length..."
	../subset --prefix-with-indices -s 5 -n 7 --input="test-input.txt"

line-numbers-file:
	@echo "Testing use of line numbers file..."
	../subset --prefix-with-indices --line-numbers="test-line-numbers.txt" --input="test-input.txt"
