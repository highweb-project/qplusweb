# Copyright (C) 2013 Apple. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import datetime
import itertools

from google.appengine.ext import webapp
from google.appengine.ext.webapp import template

from model.queues import Queue
from model.queuestatus import QueueStatus

# Fall back to simplejson, because we are still on Python 2.5.
try:
    import json
except ImportError:
    import simplejson as json


class QueueStatusJSON(webapp.RequestHandler):
    def _rows_for_work_items(self, queue):
        queued_items = queue.work_items()
        active_items = queue.active_work_items()
        if not queued_items:
            return []

        rows = []
        for item_id in queued_items.item_ids:
            statuses = QueueStatus.all().filter('queue_name =', queue.name()).filter('active_patch_id =', item_id).order('-date').fetch(1)
            status = statuses[0].message if statuses else ""
            rows.append({
                "attachment_id": item_id,
                "active": active_items and active_items.time_for_item(item_id) != None,
                "status": status,
                "status_page": self.request.host_url + "/patch/" + str(item_id),
            })
        return rows

    def _bots(self, queue):
        bot_id_statuses = QueueStatus.all(projection=['bot_id'], distinct=True).filter('queue_name =', queue.name()).fetch(500)
        bot_ids = list(entry.bot_id for entry in bot_id_statuses)
        result = []
        for bot_id in bot_ids:
            status = QueueStatus.all().filter('bot_id =', bot_id).order('-date').get()
            result.append({
                "bot_id": bot_id,
                "status_page": self.request.host_url + "/queue-status/" + queue.name() + "/bots/" + bot_id,
                "latest_message": status.message,
                "latest_message_time": status.date,
                "latest_output": self.request.host_url + "/results/" + str(status.key().id()) if status.results_file else None,
                "active_bug_id": status.active_bug_id,
                "active_patch_id": status.active_patch_id,
            })
        return result

    def get(self, queue_name):
        self.response.headers["Access-Control-Allow-Origin"] = "*"

        queue_name = queue_name.lower()
        queue = Queue.queue_with_name(queue_name)
        if not queue:
            self.error(404)
            return

        self.response.headers['Content-Type'] = 'application/json'

        status = {
            "status_page": self.request.host_url + "/queue-status/" + queue_name,
            "queue": self._rows_for_work_items(queue),
            "bots": self._bots(queue),
        }
        dthandler = lambda obj: obj.isoformat() if isinstance(obj, datetime.datetime) or isinstance(obj, datetime.date) else None
        self.response.out.write(json.dumps(status, default=dthandler))
