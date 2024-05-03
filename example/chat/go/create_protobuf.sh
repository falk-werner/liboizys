#!/bin/bash
# SPDX-License-Identifier: MPL-2.0
# SPDX-FileCopyrightText: Copyright 2024 Falk Werner

protoc -I=.. --go_out=. --go_opt=paths=source_relative "--go_opt=Mmessages.proto=.;main" messages.proto
