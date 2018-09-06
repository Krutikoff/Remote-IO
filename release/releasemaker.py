#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Utility for a making releases.
   Use --help to get help."""

from __future__ import unicode_literals

import subprocess
import datetime
import shutil
import glob
import codecs
import os
from sys import stdin
import argparse
import json
import zipfile

VERSION = "v.1.3.0_06.09.2018"
DESCRIPTION = """Utility for a making releases.

How to use this script:
  1) use --rv arg with proper version to start release, 
     it's include creating a new branch if it's not already 
     created and printing paths to all files you
     need to change like changelog, version files
     and others;
  2) update all files from the step 2 and make a commit;
  3) call script again to make release without trobles.

This script should be located in a some Git repository folder.
'release_config.json' should be provided or you can pass
another config json-file throw the '--conf' arg into this script.

A config file can/should contain such fields as:
  - "project" is a name of your project,
  - "changelog" is a path to chagelog file,
  - "output_folder" is a path where a release folder
    will be created,
  - "make_archive" — if set then an archive will be created,
  - "version_in_files_check_enable" — if enabled then some
    files from "version_in_files" section will be checked on
    correctness of an version (or it's parts) included inside
  - "version_in_files" is an array of paths to some text files
    that will be checked (see above)
    - "check_version" [optional, true] — if enabled "v.x.y.z" part
      will be checked
    - "check_date" [optional, false] — if enabled date part
      will be checked
    - "check_commit_hash" [optional, false] — if enabled commit
      hash part will be checked
    - "path" is a path file that will be checked
  - "sequence" [optional] is a sequence of commands to proceed:
    - "type" of command can be "run", "move" or "copy". For "run"
      type there are two fields required:
        * "exec" — some application to run;
        * "args" — a list of args for application above.
      For "move" and "copy" commands there are also two additional
      fields required:
        * "src" — a list of all stuff that you than to move or copy
           (it can be file or folder);
        * "dest" — a list of destinations. You can also change a name
          of a file/folder here by just writing it different than in 
          a "src".
      You can use a "${release}" placeholder to specify a path to 
      resulting release folder (see example below)

Here is config example:
{
    "project": "test_project",
    "changelog": "./changelog.txt",
    "output_folder": "./",
    "make_archive": true,
    "sequence" : [
        {
            "type" : "run",
            "exec" : "python",
            "args" : ["./test_folders/test_script.py"]
        },
        {
            "type" : "copy",
            "src" : [
                "./test_folders/test_hex_folder/project.hex",
                "./test_folders/test_hex_folder/project_another.hex",
                "./test_folders/test_random_stuff_folder/random_stuff",
                "./test_folders/test_random_stuff_folder/stuff.txt"
            ],
            "dest" : [
                "${release}/project.hex",
                "${release}/project_another.hex",
                "${release}/random_stuff",
                "${release}/stuff.txt"
            ]
        },
        {
            "type" : "move",
            "src" : [
                "./test_folders/test_folder_to_move"
            ],
            "dest" : [
                "${release}/test_moved_folder"
            ]
        }
    ],
    "version_in_files_check_enable": true,
    "version_in_files": [
        {
            "check_version": true,
            "check_date": true,
            "check_commit_hash": false,
            "path": "./test_folders/test_version_folder/version.h"
        },
        {
            "check_version": true,
            "check_date": false,
            "check_commit_hash": true,
            "path": "./test_folders/test_another_version_folder/version.h"
        }
    ]
}
"""


class Error(Exception):
    """Base class for exceptions in this module."""
    def __init__(self, msg, last_err=None):
        if (last_err is None):
            last_err = ""
        else:
            last_err = "\nError \'{}\': {}".format(
                type(last_err).__name__, str(last_err).replace("\n", ""))
        super(Error, self).__init__("{}{}".format(msg, last_err))


class GitNotFoundInPath(Error):
    """Git app was not found in PATH."""
    def __init__(self, last_err):
        super(GitNotFoundInPath, self).__init__(
              "Git app was not found in PATH!"
              "Try to add path to python into your env.", last_err)


class GitError(Error):
    """Git error."""
    def __init__(self, msg, git_msg):
        super(GitError, self).__init__(
              "Git error occurred!\n"
              "{}\n{}".format(msg, git_msg))


class GitFlowReleaseStartFail(GitError):
    """Fail to create release branch."""
    def __init__(self, git_msg):
        super(GitFlowReleaseStartFail, self).__init__(
              "Fail to create release branch!", git_msg)


class GitRepoNotFound(GitError):
    """Git repository not found."""
    def __init__(self, git_msg):
        super(GitRepoNotFound, self).__init__(
              "Git repository not found at the script location!\n"
              "You have to start the script only from a some "
              "repository folder.", git_msg)


class BadConfigFormat(Error):
    """Bad config format."""
    def __init__(self, last_err):
        super(BadConfigFormat, self).__init__(
              "Bad config format! Check your config.json file.", last_err)


class ConfigFieldNotFound(Error):
    """Some config field is not found"""
    def __init__(self, field_name):
        super(ConfigFieldNotFound, self).__init__(
              "Field \"{}\" is required! "
              "Check your config.json file.".format(field_name))


class ConfigFieldHasWrongType(Error):
    """Some config field has wrong type"""
    def __init__(self, field_name):
        super(ConfigFieldHasWrongType, self).__init__(
              "Field \"{}\" has wrong type! "
              "Check your config.json file.".format(field_name))


class VersionInFileNotEqualToGit(Error):
    """Version contained in file is not equal to a git version."""
    def __init__(self, file_name):
        super(VersionInFileNotEqualToGit, self).__init__(
            "Version contained in {} is not equal to a git version!"
            .format(file_name))


class ChangelogNotChanged(Error):
    """Changelog does not contain a current version changes description."""
    def __init__(self):
        super(ChangelogNotChanged, self).__init__(
            "Changelog does not contain a current "
            "version changes description!")


class ChangelogNotFound(Error):
    """changelog.txt file not found"""
    def __init__(self):
        super(ChangelogNotFound, self).__init__(
            "changelog.txt file not found!")


class PathNotFound(Error):
    """Path provided by config is not found."""
    def __init__(self, last_err):
        super(PathNotFound, self).__init__(
            "Path provided by config is not found!", last_err)


class NotActualDate(Error):
    """Date found in version is not actual."""
    def __init__(self):
        super(NotActualDate, self).__init__(
              "Date found in version is not actual!")


class NotReleaseBranch(Error):
    """HEAD not on release branch."""
    def __init__(self):
        super(NotReleaseBranch, self).__init__(
              "You can not make a release not on a release branch!")


class Defaults:
    CHANGELOG_NAME = 'changelog.txt'
    CONFIG_NAME = 'release_config.json'


def _path_leaf(path):
    head, tail = os.path.split(path)
    return tail or os.path.basename(head)


def _get_or_raise(d, key):
    value = d.get(key, None)
    if value is None:
        raise ConfigFieldNotFound(key)
    return value


class _VersionInFiles(object):
    def __init__(self, init_struct):
        if type(init_struct) is not list:
            raise ConfigFieldHasWrongType("version_in_files")

        self.files = init_struct

        opt_keys = {"check_date": False,
                    "check_commit_hash": False,
                    "check_version": True}

        req_keys = {"path": ""}

        for f in self.files:
            keys = [k for k in f.keys()]
            keys_for_type_check = []

            for k in opt_keys.keys():
                if k not in keys:
                    f[k] = opt_keys[k]
                else:
                    keys_for_type_check.append(k)

            for k in req_keys.keys():
                if k not in keys:
                    raise ConfigFieldNotFound(k)
                else:
                    keys_for_type_check.append(k)

            for k in keys_for_type_check:
                if k in req_keys:
                    if not isinstance(req_keys[k], type(f[k])):
                        raise ConfigFieldHasWrongType(k)
                else:
                    if not isinstance(opt_keys[k], type(f[k])):
                        raise ConfigFieldHasWrongType(k)

    def get_strs_to_check(self, f, version, date, commit_hash):
        local = {"strs": [], "is_split_next": True}

        def add_or_split(str, cond, delimiter=""):
            if (cond):
                if local["is_split_next"]:
                    local["strs"].append(str)
                    local["is_split_next"] = False
                else:
                    local["strs"][-1] += delimiter + str
            else:
                local["is_split_next"] = True

        add_or_split(version, f["check_version"])
        add_or_split(date, f["check_date"], "_")
        add_or_split(commit_hash, f["check_commit_hash"], "__")

        return local["strs"]


class ConfigSeqFieldNotFound(Error):
    """Some sequence's config field was not found"""
    def __init__(self, field_name):
        super(ConfigSeqFieldNotFound, self).__init__(
              "Field \"{}\" inside the \"sequence\" field is skipped! "
              "Check your config.json file.".format(field_name))


class ConfigSeqFieldHasWrongType(Error):
    """Some sequence's config field has wrong type"""
    def __init__(self, field_name, req_type):
        super(ConfigSeqFieldHasWrongType, self).__init__(
              "Field \"{}\" inside the \"sequence\" field has wrong type! "
              "Required type is \"{}\". "
              "Check your config.json file.".format(field_name, req_type))


class SrcAndDestFieldsLenNotEqual(Error):
    """"src" and "dest" lists length must be equal"""
    def __init__(self):
        super(SrcAndDestFieldsLenNotEqual, self).__init__(
              "\"src\" and \"dest\" lists length must be equal!")


class CommandBadFinish(Error):
    """Command from some step return error code"""
    def __init__(self, msg):
        super(CommandBadFinish, self).__init__(
              "Command from some step return error code! "
              "Command output:\n\n{}".format(msg))


class StartFinishError(Error):
    """Bad --start or --finish args"""
    def __init__(self):
        super(StartFinishError, self).__init__(
              "You must use the only one --start or --finish arg!")

class VersionIsNotEqualToGit(Error):
    """Version provided by arg -rv is not equal to git"""
    def __init__(self):
        super(VersionIsNotEqualToGit, self).__init__(
              "Version provided by arg -rv is not equal to git!")


class _ReleaseSequence(object):
    CONFIG_STR = "sequence"

    STEPS = {
        "run": ("exec", "args"),
        "copy": ("src", "dest"),
        "move": ("src", "dest"),
        "rm": ("src",)
    }

    STEP_FIELDS_TYPES = {
        "type": "str",
        "args": [],
        "exec": "str",
        "src": [],
        "dest": []
    }

    DEFAULT_RELEASE_PATH = "_release_"

    class FsAction:
        RM = 1
        MOVE = 2
        COPY = 3

    def __init__(self, seq_steps):
        if type(seq_steps) is not list:
            raise ConfigFieldHasWrongType(_ReleaseSequence.CONFIG_STR)

        for step in seq_steps:
            self.__check_step_format(step)

        self.__steps = seq_steps

        self.__handlers = {
            "run": (self.__proc_run_step, _ReleaseSequence.STEPS["run"]),
            "copy": (self.__proc_copy_step, _ReleaseSequence.STEPS["copy"]),
            "move": (self.__proc_move_step, _ReleaseSequence.STEPS["move"]),
            "rm": (self.__proc_rm_step, _ReleaseSequence.STEPS["rm"])
        }

        self.__release_path = _ReleaseSequence.DEFAULT_RELEASE_PATH

    def proceed(self):
        if self.__release_path == _ReleaseSequence.DEFAULT_RELEASE_PATH:
            if os.path.exists(self.__release_path):
                shutil.rmtree(self.__release_path, ignore_errors=True)
            os.mkdir(self.__release_path)

        for step in self.__steps:
            h = self.__handlers[step["type"]]
            args = [step[f] for f in h[1]]
            h[0](*args)

    @property
    def release_path(self):
        return self.__release_path

    @release_path.setter
    def release_path(self, path):
        self.__release_path = path

    def __get_or_raise(self, d, key):
        value = d.get(key, None)
        if value is None:
            raise ConfigSeqFieldNotFound(key)
        return value

    def __check_step_format(self, step):
        type_field = self.__get_or_raise(step, "type")
        if type_field not in _ReleaseSequence.STEPS.keys():
            raise ConfigSeqFieldHasWrongType(
                "type", list(_ReleaseSequence.STEPS.keys()))

        for f in _ReleaseSequence.STEPS[type_field]:
            val = self.__get_or_raise(step, f)
            val_sample = _ReleaseSequence.STEP_FIELDS_TYPES[f]
            if not isinstance(val, type(val_sample)):
                raise ConfigSeqFieldHasWrongType(f, type(val_sample).__name__)

    def __proc_run_step(self, executable, args):
        command = [executable] + args
        print("\nCommand start: \"{}\"".format(' '.join(command)))
        process = subprocess.Popen(
            command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        _, errout = process.communicate()
        if process.returncode != 0:
            raise CommandBadFinish(errout.decode("utf-8", 'ignore'))
        print("...command finished.")

    def __proc_copy_step(self, srcs, dests):
        self.__proc_fs_actions(srcs=srcs, dests=dests, 
                               type=_ReleaseSequence.FsAction.COPY)

    def __proc_move_step(self, srcs, dests):
        self.__proc_fs_actions(srcs=srcs, dests=dests, 
                               type=_ReleaseSequence.FsAction.MOVE)

    def __proc_rm_step(self, srcs):
        self.__proc_fs_actions(srcs=srcs, dests=[], 
                               type=_ReleaseSequence.FsAction.RM)

    def __proc_fs_actions(self, srcs, dests, type):
        is_rm_action = (type == _ReleaseSequence.FsAction.RM)
        is_remove_at_the_end = (is_rm_action or type == _ReleaseSequence.FsAction.MOVE)

        if not is_rm_action:
            if len(srcs) != len(dests):
                raise SrcAndDestFieldsLenNotEqual()

        for i in range(0, len(srcs)):
            src = srcs[i]
            if not is_rm_action:
                dest = dests[i]

            src = src.replace("${release}", self.__release_path)
            if not is_rm_action:
                dest = dest.replace("${release}", self.__release_path)

            if (os.path.isfile(src)):
                if not is_rm_action:
                    shutil.copy(src, dest)
                if is_remove_at_the_end:
                    os.remove(src)
            else:
                if not is_rm_action:
                    shutil.copytree(src, dest)
                if is_remove_at_the_end:
                    shutil.rmtree(src)


class ReleaseMaker(object):
    DEFAULT_DEBUG_ARGS = {
        "is_branch_checking_off": False,
        "is_changelog_checking_off": False,
        "is_date_checking_off": False
    }

    def __init__(self, conf_path, release_version, debug_args=DEFAULT_DEBUG_ARGS):
        self.__conf_path = conf_path
        self.__conf = self.__parse_config()

        self.__debug_args = debug_args
        self.__release_ver = release_version

    def __make_full_version(self, version, commit_hash=None):
        if commit_hash is not None:
            return "v.{}__{}".format(version, commit_hash)
        else:
            return "v.{}".format(version)

    def __parse_config(self):
        try:
            with codecs.open(self.__conf_path, 'r', 'utf-8') as conf_file:
                conf = json.load(conf_file)
        except ValueError as e:
            raise BadConfigFormat(e)

        self.__project_name = _get_or_raise(conf, "project")
        self.__changelog = _get_or_raise(conf, "changelog")
        self.__output_folder = _get_or_raise(conf, "output_folder")
        self.__is_make_archive = conf.get("make_archive", True)

        is_ver_h_check_enabled =\
            _get_or_raise(conf, "version_in_files_check_enable")
        if (is_ver_h_check_enabled):
            self.__version_in_files =\
                _VersionInFiles(_get_or_raise(conf, "version_in_files"))
        else:
            self.__version_in_files = None

        self.__sequence = _ReleaseSequence(_get_or_raise(conf, "sequence"))

        return conf

    def __check_git_repo_existing(self):
        try:
            process = subprocess.Popen(
                ["git", "status"], 
                stdout=subprocess.PIPE, stderr=subprocess.PIPE)

            _, errout = process.communicate()

            if process.returncode != 0:
                raise GitRepoNotFound(errout.decode("utf-8", 'ignore'))
        except OSError as e:
            raise GitNotFoundInPath(e)

    def __get_git_responce(self):
        self.__check_git_repo_existing()
        p = subprocess.Popen('git describe --always --dirty=+'
                             ' --abbrev=5 &&'
                             'git symbolic-ref --short HEAD',
                             stdout=subprocess.PIPE, shell=True)

        git_responce = p.communicate()[0].decode().split('\n')
        git_describe, branch_name = git_responce[:2]
        last_commit_hash = git_describe.split('-')[-1]

        version = branch_name.split('/')[-1].replace('v.', '')

        return version, last_commit_hash, branch_name

    def __check_string(self, file_path, string):
        with codecs.open(file_path, encoding='utf-8') as file_with_version:
            f_str = file_with_version.read().replace('\n', '')

        if f_str.find(string) != -1:
            return True
        else:
            return False

    def __check_program_versions(self, full_version):
        if self.__version_in_files is None:
            return

        full_version = full_version.replace('__', '_')
        version, date, commit_hash = full_version.split("_")
        version = version.replace('v.', '')

        files = self.__version_in_files.files
        for f in files:
            strs = self.__version_in_files.get_strs_to_check(f, version, date,
                                                             commit_hash)
            for s in strs:
                is_str_found = self.__check_string(f["path"], s)
                if not is_str_found:
                    raise VersionInFileNotEqualToGit(f["path"])

    def __check_changelog(self, version_without_commit_hash):
        try:
            with codecs.open(self.__changelog, encoding='utf-8') as changelog:
                changelog_str = changelog.read().replace('\n', '')
        except IOError:
            raise ChangelogNotFound()

        if changelog_str.find(version_without_commit_hash) == -1:
            raise ChangelogNotChanged()

    def __check_date(self, version):
        date = datetime.date.today().strftime('%d.%m.%Y')
        if date != version.split('_')[1]:
            raise NotActualDate()

    def __check_version_equal_to_git(self, version):
        if version.find(self.__release_ver) == -1:
            raise VersionIsNotEqualToGit()

    def __copy_changelog(self, output):
        shutil.copyfile(
            self.__changelog,
            os.path.join(output,
                         _path_leaf(self.__changelog)))

    def __git_start_release(self):
        date = datetime.date.today().strftime('%d.%m.%Y')
        release_branch_name = "v.{}_{}".format(self.__release_ver, date)

        process = subprocess.Popen(
            ["git", "flow", "release", "start", release_branch_name], 
            stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        _, errout = process.communicate()

        if process.returncode != 0:
            raise GitFlowReleaseStartFail(errout.decode("utf-8", 'ignore'))

    def prepare_release(self):
        (version, _, branch_name) = self.__get_git_responce()
        if branch_name.find("release/") == -1:
            self.__git_start_release()
            (version, _, _) = self.__get_git_responce()
            print('Release was started!\n')
        else:
            self.__check_version_equal_to_git(version)
            print('Continue release...\n')
        
        if self.__version_in_files is not None:
            print("Files that should contain the correct version:")
            for f in self.__version_in_files.files:
                print('  {}'.format(f['path']))
            print('')

    def make_release(self):
        (version, commit_hash, branch_name) = self.__get_git_responce()

        if not self.__debug_args["is_branch_checking_off"]:
            if branch_name.find("release/") == -1:
                raise NotReleaseBranch()

        full_release_version = self.__make_full_version(version, commit_hash)
        release_folder_name = os.path.join(self.__output_folder,
                                           full_release_version)

        print('Current release version of'
              ' \"{}\" project is: {}'.format(self.__project_name,
                                              full_release_version))

        self.__sequence.release_path = release_folder_name

        try:
            self.__check_version_equal_to_git(version)

            if not self.__debug_args["is_date_checking_off"]:
                self.__check_date(version)
            
            if not self.__debug_args["is_changelog_checking_off"]:
                self.__check_changelog(version)

            self.__check_program_versions(full_release_version)

            if os.path.exists(release_folder_name):
                shutil.rmtree(release_folder_name, ignore_errors=True)
            os.mkdir(release_folder_name)

            self.__copy_changelog(release_folder_name)
            self.__sequence.proceed()

            if self.__is_make_archive:
                zf = zipfile.ZipFile("{}.zip".format(release_folder_name), "w")
                for dirname, subdirs, files in os.walk(release_folder_name):
                    zf.write(dirname)
                    for filename in files:
                        zf.write(os.path.join(dirname, filename))
                zf.close()

            print('\nScript finished!')
        except IOError as e:
            shutil.rmtree(release_folder_name, ignore_errors=True)
            raise PathNotFound(e)
        except:
            shutil.rmtree(release_folder_name, ignore_errors=True)
            raise

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='releasemaker',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=DESCRIPTION)

    parser.add_argument('--version',
                        action='version',
                        version='%(prog)s {}'.format(VERSION))

    parser.add_argument('--conf',
                        help='path to \'config.json\' file.',
                        dest='conf_path',
                        default=Defaults.CONFIG_NAME)

    parser.add_argument('--release_version', '-rv',
                        help='current release version (like 1.2.3).',
                        dest='release_version',
                        required=True)

    parser.add_argument('--turn_off_branch_checking',
                        action='store_true',
                        help='[FOR DEBUG ONLY].',
                        dest='is_branch_checking_off',
                        default=False)

    parser.add_argument('--turn_off_changelog_checking',
                        action='store_true',
                        help='[FOR DEBUG ONLY].',
                        dest='is_changelog_checking_off',
                        default=False)

    parser.add_argument('--turn_off_date_checking',
                        action='store_true',
                        help='[FOR DEBUG ONLY].',
                        dest='is_date_checking_off',
                        default=False)

    parser.add_argument('--debug',
                        action='store_true',
                        help='[FOR DEBUG ONLY].',
                        dest='is_debug_mode_on',
                        default=False)

    args = parser.parse_args()

    debug_args = ReleaseMaker.DEFAULT_DEBUG_ARGS.copy()
    if args.is_debug_mode_on:
        debug_args["is_branch_checking_off"] = args.is_branch_checking_off
        debug_args["is_changelog_checking_off"] = args.is_changelog_checking_off
        debug_args["is_date_checking_off"] = args.is_date_checking_off

    try:
        app = ReleaseMaker(args.conf_path, args.release_version, debug_args)
        app.prepare_release()
        app.make_release()

    except Error as e:
        print("\n{}\n\nUse --help to get help.\n".format(e))
