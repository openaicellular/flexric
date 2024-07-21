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
    generate_subchapter,
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
    chapterSummary = ''
    subChapterSummary = ''

    if os.path.isfile(f'{cwd}/archives/{reportName}'):
        status = True
        section_start_pattern = 'Test project /flexric/build'
        section_end_pattern = 'Total Test time'
        section_status = False
        chapterSummary = generate_list_header()
        with open(f'{cwd}/archives/{reportName}', 'r') as logfile:
            for line in logfile:
                sanitizer = re.search('(Selected SANITIZER TYPE: [A-Z_]+)', line)
                if sanitizer is not None:
                    subChapterName = sanitizer.group(1)[25:] + ' sanitizer'
                    subChapterSummary = generate_list_header()
                e2ap_version = re.search('(Selected E2AP_VERSION: [A-Z0-9_]+)', line)
                if e2ap_version is not None:
                    subChapterName += ' & ' + e2ap_version.group(1)[23:]
                kpm_version = re.search('(Selected KPM Version: [A-Z0-9_]+)', line)
                if kpm_version is not None:
                    subChapterName += ' & ' + kpm_version.group(1)[22:]
                if re.search(section_start_pattern, line) is not None and not section_status:
                    section_status = True
                if section_status and re.search(section_end_pattern, line) is not None:
                    section_status = False
                    subChapterSummary += generate_list_footer()
                    subchapter = generate_subchapter(subChapterName) + subChapterSummary
                    chapterSummary += subchapter
                if section_status:
                    result = re.search('(Unit_test_[A-Za-z0-9_]+) [\.]+', line)
                    passed = re.search('Passed', line)
                    if result is not None and passed is not None:
                        subChapterSummary += generate_list_row(result.group(1)[10:], 'thumbs-up')
                    elif result is not None:
                        subChapterSummary += generate_list_row(result.group(1)[10:], 'remove')
                        status = False
        chapterSummary += generate_list_footer()
    else:
        chapterSummary = generate_chapter(chapterName, 'CTests report file not found! Not run?', False)
        return chapterSummary
    if status:
        chapterSummary = generate_chapter(chapterName, 'All CTests passed', True) + chapterSummary
    else:
        chapterSummary = generate_chapter(chapterName, 'Some CTests failed', False) + chapterSummary
    return chapterSummary

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
