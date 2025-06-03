import pandas as pd
import io
import sys
import os
from multiprocessing import shared_memory

def process_shared_memory_to_parquet(shm_name, output_path):
    try:
        # Attach to the existing shared memory block
        shm = shared_memory.SharedMemory(name=shm_name)
        
        # Read the shared memory buffer as bytes and decode to string
        csv_data = shm.buf.tobytes().decode('utf-8')
        
        # Create a string buffer from the CSV data
        buffer = io.StringIO(csv_data)
        
        # Read CSV from buffer into pandas DataFrame
        df = pd.read_csv(buffer)
        
        # Write DataFrame to Parquet file
        df.to_parquet(output_path, engine='pyarrow', index=False)
        
        # Close shared memory
        shm.close()
        
        return f"Successfully wrote data from shared memory '{shm_name}' to {output_path}"
    
    except Exception as e:
        return f"Error: {str(e)}"
    finally:
        # Ensure shared memory is closed
        if 'shm' in locals():
            shm.close()

def main(input_file, output_file):
    try:
        # Derive shared memory name from input file (use base name without extension)
        shm_name = os.path.splitext(os.path.basename(input_file))[0]
        
        if output_file.startswith('s3://'):
            # Split by '/' and take the last part as the file name
            output_file_name = output_file.split('/')[-1]
        else:
            output_file_name = output_file
                
        
        # Read CSV data from input file
        with open(input_file, 'r', encoding='utf-8') as f:
            csv_data = f.read()
        
        # Create shared memory block and load CSV data
        shm = shared_memory.SharedMemory(create=True, size=len(csv_data.encode('utf-8')), name=shm_name)
        shm.buf[:len(csv_data)] = csv_data.encode('utf-8')
        
        # Process the shared memory data to Parquet
        result = process_shared_memory_to_parquet(shm_name, output_file_name)
        print(result)
        
    except Exception as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        sys.exit(1)
    finally:
        # Clean up shared memory
        if 'shm' in locals():
            shm.close()
            shm.unlink()

# Command-line execution
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python lambda.py <input_csv_file> <output_parquet_file>",
              file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    main(input_file, output_file)
