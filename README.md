# ClassBench-ng
A tool for generation of synthetic classification rule sets for benchmarking, which is based on original (no longer maintained) [ClassBench](http://www.arl.wustl.edu/classbench/).
The format of the generated rules can be one of the following:
- IPv4 5-tuple
- IPv6 5-tuple
- OpenFlow

## Requirements
- Python3
- Ruby 1.9.3+
- RubyGems
```
sudo gem install open4 ruby-ip docopt ipaddress
```

## Installation
```
cd ./db_generator
make all
cd ../trace_generator
make all
```

## Usage
ClassBench-ng can be used in two different ways:
- To analyse an existing rule set and extract a corresponding SEED. (not included in this repository)
- To generate a synthetic rule set from an input SEED.

```
./classbench -h | --help
```
Prints detailed usage information.

### ClassBench-ng Analyser (NOT INCLUDED IN THIS REPOSITORY)
The current version can successfully analyse IPv4 5-tuples, IPv6 5-tuples, and OpenFlow rules.

```
./classbench analyse tuples FILE FORMAT [-l]
```
Analyses FILE, expecting FILE to be in the format specified in FORMAT (see `./lib/tuples_analyzer/README` for more information on how to specify the format).
- `-l` enables printing analysis error logs.

The output is an original ClassBench seed.

```
./classbench analyse of FILE
```
Analyses FILE, expecting FILE to be in the format used by `ovs-ofctl`.
Fields extracted from FILE are:
- in_port,
- dl_src, dl_dst, eth_type, dl_vlan, dl_vlan_pcp,
- nw_src, nw_dst, nw_tos, nw_proto,
- tp_src, tp_dst

The output is an original ClassBench seed with an OpenFlow YAML structure as the last section.

### ClassBench-ng Rule Generator
The current version can successfully generate IPv4, IPv6 and OpenFlow 1.0 flow rules.
- IPv4/IPv6 SEEDs can be found in `./parameter_files`
- OpenFlow SEEDs can be found in `./seeds`

```
./classbench generate v4 SEED [--count=<n>] [--db-generator=<path>]
```
Generates IPv4 5-tuples following the properties from SEED.
- `--count=<n>` specifies the number of 5-tuples to be generated (default: `100`)
- `--db-generator=<path>` specifies the path to an original ClassBench binary (default: `.db_generator/db_generator`)

The output format is the same as of original ClassBench outputs.

```
./classbench generate v6 SEED [--count=<n>] [--db-generator=<path>]
```
Generates IPv6 5-tuples following the properties from SEED.
- `--count=<n>` specifies the number of 5-tuples to be generated (default: `100`)
- `--db-generator=<path>` specifies the path to an original ClassBench binary (default: `./db_generator/db_generator`)

The output format is the same as of original ClassBench outputs.

```
./classbench generate of SEED [--count=<n>] [--db-generator=<path>]
```
Generates OpenFlow rules following the properties from SEED that has to contain an OpenFlow section.
- `--count=<n>` specifies the number of rules to be generated (default: `100`)
- `--db-generator=<path>` specifies the path to an original ClassBench binary (default: `./db_generator/db_generator`)

The output consists of `attribute=value` pairs joined by `, `.

## Known Issues
- the number of generated rules is usually lower than in original ClassBench (i.e., ClassBench-ng generates higher number of redundant rules that are removed in the last phase)
- ClassBench-ng Analyser does not correctly analyses source/destination port prefixes specified using a bit map in the ovs-ofctl format

## How to Contribute
Contributions are welcome via:
- pull requests (preferred)
- e-mail to imatousek at fit.vutbr.cz
