import json
import sys


def generate_table_from_dict(data_dict, title=None):
    rst = ""
    if title:
        rst += f"{title}\n{'-' * len(title)}\n\n"

    rst += ".. list-table::\n"
    rst += "   :widths: auto\n"
    rst += "   :header-rows: 1\n\n"

    rst += "   * - Key\n"
    rst += "     - Value\n"

    for key, value in data_dict.items():
        if key == 'caches':
            rst += f"   * - {key}\n\n"
            rst += "     - .. list-table::\n\n"
            rst += "        * - Type\n"
            rst += "          - Level\n"
            rst += "          - Size (Bytes)\n"
            rst += "          - Num Sharing\n"
            for cache in value:
                rst += f"        * - {cache['type']}\n"
                rst += f"          - {cache['level']}\n"
                rst += f"          - {cache['size']}\n"
                rst += f"          - {cache['num_sharing']}\n"
        else:
            formatted_value = json.dumps(value) if isinstance(value, (list, dict)) else str(value)
            rst += f"   * - {key}\n"
            rst += f"     - {formatted_value}\n"

    rst += "\n"
    return rst


def generate_benchmarks_table(benchmarks):
    if not benchmarks:
        return ""

    headers_to_remove = ['family_index', 'per_family_instance_index', 'run_name', 'run_type', 'repetitions',
                         'repetition_index', 'threads', 'time_unit']

    rst = "Benchmarks\n----------\n\n.. list-table::\n   :widths: auto\n   :header-rows: 1\n\n   * - Index\n"
    header_added = False

    for benchmark in benchmarks:
        if not header_added:
            for key in benchmark.keys():
                if key not in headers_to_remove:
                    new_key = key.replace('name', 'Benchmark').replace('real_time', 'Real Time (\u00B5s)').replace(
                        'cpu_time', 'CPU Time (\u00B5s)').replace('iterations', 'Iterations')
                    rst += f"     - {new_key}\n"
            header_added = True

        rst += "   * - " + str(benchmark['family_index'] + 1) + "\n"
        for key, value in benchmark.items():
            if key not in headers_to_remove:
                if key == 'name':
                    value = value.replace('OASValidatorPerf/', '').split('/min_time')[0]
                elif key in ['real_time', 'cpu_time']:
                    value = f"{value:.3f}"
                rst += f"     - {value}\n"
    return rst + "\n"


def generate_rst():
    file_path = sys.argv[1]
    with open(file_path, "r") as file:
        data = json.load(file)
    rst = f"Benchmark Report\n{'=' * len('Benchmark Report')}\n\n"
    rst += generate_table_from_dict(data["context"], "Context")
    rst += generate_benchmarks_table(data["benchmarks"])
    return rst


rst_document = generate_rst()

# Save the RST to a file
with open(sys.argv[2], "w") as file:
    file.write(rst_document)

print("RST document generated and saved as " + sys.argv[2] + ".")
