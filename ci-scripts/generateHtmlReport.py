#!/usr/bin/env python3
"""
Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
contributor license agreements.  See the NOTICE file distributed with
this work for additional information regarding copyright ownership.
The OpenAirInterface Software Alliance licenses this file to You under
the OAI Public License, Version 1.1  (the "License"); you may not use this file
except in compliance with the License.
You may obtain a copy of the License at

  http://www.openairinterface.org/?page_id=698

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
------------------------------------------------------------------------------
For more information about the OpenAirInterface (OAI) Software Alliance:
  contact@openairinterface.org
---------------------------------------------------------------------
"""

#import argparse
import datetime
import os
import re

from common.python.pipeline_args_parse import (
    _parse_args,
)

from common.python.generate_html import (
    generate_header,
    generate_footer,
    generate_git_info,
    generate_chapter,
    generate_list_header,
    generate_list_footer,
    generate_list_row,
)

from common.python.code_format_checker import (
    coding_formatting_log_check,
)

from common.python.static_code_analysis import (
    analyze_sca_log_check,
)

from common.python.building_report import (
    build_summary,
)

REPORT_NAME = 'test_results_oai_flexric.html'

def ctest_summary(args, reportName):
    cwd = os.getcwd()
    status = True
    chapterName = 'CTests Summary'
    summary = ''
    if os.path.isfile(f'{cwd}/archives/{reportName}'):
        status = True
        section_start_pattern = 'Test project /flexric/build'
        section_end_pattern = 'Total Test time'
        section_status = False
        summary = generate_list_header()
        with open(f'{cwd}/archives/{reportName}', 'r') as logfile:
            for line in logfile:
                if re.search(section_start_pattern, line) is not None and not section_status:
                    section_status = True
                if section_status and re.search(section_end_pattern, line) is not None:
                    section_status = False
                if section_status:
                    result = re.search('(Test *#[0-9]+: Unit_test_[A-Za-z0-9_]+) [\.]+', line)
                    passed = re.search('Passed', line)
                    if result is not None and passed is not None:
                        summary += generate_list_row(result.group(1), 'thumbs-up')
                    elif result is not None:
                        summary += generate_list_row(result.group(1), 'thumbs-down')
        summary += generate_list_footer()
    else:
        summary = generate_chapter(chapterName, 'CTests report file not found! Not run?', False)
        return summary
    if status:
        summary = generate_chapter(chapterName, 'All CTests passed', True) + summary
    else:
        summary = generate_chapter(chapterName, 'Some CTests failed', False) + summary
    return summary

class HtmlReport():
    def __init__(self):
        pass

    def generate(self, args):
        date = datetime.date.today()
        year = date.strftime("%Y")
        cwd = os.getcwd()
        with open(os.path.join(cwd, REPORT_NAME), 'w') as wfile:
            wfile.write(re.sub('Core Network Test ', 'FlexRIC Test', generate_header(args)))
            wfile.write(generate_git_info(args))
            wfile.write(build_summary(args, 'flexric', '22', '9'))
            wfile.write(ctest_summary(args, 'flexric_ctests.log'))
            wfile.write(re.sub('2023', year, generate_footer()))

if __name__ == '__main__':
    # Parse the arguments
    args = _parse_args()

    # Generate report
    HTML = HtmlReport()
    HTML.generate(args)
