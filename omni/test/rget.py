import sys
import zlib
import numpy as np
import pandas as pd
import pyarrow as pa
import pyarrow.parquet as pq
import os

def decompress_deflate_to_numpy(file_path, dtype=np.uint8):
    """
    Decompresses data from a binary file using Deflate and
    loads it into a NumPy array.

    Args:
        file_path (str): The path to the binary file
                         containing Deflate compressed data.
        dtype (numpy.dtype, optional): The data type for the NumPy array.
                                       Defaults to np.uint8
                                       (unsigned 8-bit integer).

    Returns:
        numpy.ndarray: The decompressed data as a NumPy array,
                       or None if an error occurs.
    """
    try:
        with open(file_path, 'rb') as f:
            compressed_data = f.read()

        # Decompress the data using zlib.decompress
        decompressed_data = zlib.decompress(compressed_data,
                                            wbits=zlib.MAX_WBITS)

        # Convert the decompressed bytes to a NumPy array
        # If you don't specify count, you will get 10,000 rows in parquet.
        numpy_array = np.frombuffer(decompressed_data, dtype=dtype, count=1185)
        return numpy_array

    except FileNotFoundError:
        print(f"Error: File not found at {file_path}")
        return None
    except zlib.error as e:
        print(f"Error during decompression: {e}")
        return None
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        return None

def save_numpy_to_parquet_pyarrow(numpy_array, file_path,
                                  column_name="data"):
    """
    Writes a 1D NumPy array to a Parquet file using PyArrow.

    Args:
        numpy_array (numpy.ndarray): The 1D NumPy array to write.
        file_path (str): The path to the output Parquet file
                          (e.g., "output.parquet").
        column_name (str): The name of the column in the Parquet file.
                           Defaults to "data".
    """
    if numpy_array.ndim != 1:
        print("Warning: PyArrow Table from single array generally expects 1D array. Reshaping might be needed for multi-dim.")
        # For multi-dimensional arrays, you might need to flatten or reshape
        # and then define a schema with appropriate types.
        # For simplicity, this example assumes a 1D array.

    try:
        # Create a PyArrow Array from the NumPy array
        arrow_array = pa.array(numpy_array)

        # Create a PyArrow Table from the Array
        # We need to give it a schema with a column name
        schema = pa.schema([pa.field(column_name, arrow_array.type)])
        table = pa.Table.from_arrays([arrow_array], schema=schema)

        # Write the PyArrow Table to a Parquet file
        pq.write_table(table, file_path)
        print(f"NumPy array successfully written to Parquet file: {file_path}")

    except Exception as e:
        print(f"Error writing Parquet file with PyArrow: {e}")
        
def save_numpy_to_parquet_pandas(numpy_array, parquet_file_path,
                                 column_name="data"):
    """
    Saves a NumPy array to a Parquet file.

    Args:
        numpy_array (numpy.ndarray): The NumPy array to save.
        parquet_file_path (str): The path where the Parquet file will be saved.
        column_name (str, optional): The name of the column in the Parquet file.
                                     Defaults to "data".
    """
    try:
        # Parquet is a columnar format, so we typically need to represent
        # the data as a table (e.g., a Pandas DataFrame or PyArrow Table).
        # For a 1D NumPy array, we can convert it to a Pandas DataFrame
        # with a single column.
        df = pd.DataFrame({column_name: numpy_array})

        # Convert Pandas DataFrame to a PyArrow Table
        table = pa.Table.from_pandas(df)

        # Write the PyArrow Table to a Parquet file
        pq.write_table(table, parquet_file_path)
        print(f"Successfully saved NumPy array to {parquet_file_path}")

    except Exception as e:
        print(f"Error saving NumPy array to Parquet: {e}")

def read_parquet_to_numpy(parquet_file_path):
    """
    Reads a Parquet file and loads the first column into a NumPy array.

    Args:
        parquet_file_path (str): The path to the Parquet file.

    Returns:
        numpy.ndarray: The data from the first column as a NumPy array,
                       or None if an error occurs.
    """
    try:
        table = pq.read_table(parquet_file_path)
        # Assuming the data is in the first column
        numpy_array = table.column(0).to_numpy()
        return numpy_array
    except Exception as e:
        print(f"Error reading Parquet file: {e}")
        return None

def main(input_file, output_file):
    if output_file.startswith('s3://'):
        # Split by '/' and take the last part as the file name
        output_file_name = output_file.split('/')[-1]
    else:
        output_file_name = output_file

    # Change dtype according the HDF5 DMR++ content map.
    decompressed_array = decompress_deflate_to_numpy(input_file,
                                                     dtype=np.float64)

    if decompressed_array is not None:
        print("\nDecompressed NumPy Array:")
        # print(decompressed_array)
        
        # Pandas
        # save_numpy_to_parquet_pandas(decompressed_array, output_file_name)
        
        # Arrow
        save_numpy_to_parquet_pyarrow(decompressed_array, output_file_name)
        

        print("\nReading data back from Parquet file:")
        loaded_array = read_parquet_to_numpy(output_file_name)
        if loaded_array is not None:
            print(loaded_array)
            if np.array_equal(decompressed_array, loaded_array):
                print("Data read from Parquet matches the decompressed array.")
            else:
                print("Data read from Parquet DOES NOT match the decompressed array.")


# Command-line execution
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python lambda.py <input_csv_file> <output_parquet_file>",
              file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    main(input_file, output_file)
        
