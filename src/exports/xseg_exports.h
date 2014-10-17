/*
 * Copyright (C) 2010-2014 GRNET S.A.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

EXPORT_SYMBOL(xseg_initialize);
EXPORT_SYMBOL(xseg_finalize);
EXPORT_SYMBOL(xseg_parse_spec);
EXPORT_SYMBOL(xseg_register_type);
EXPORT_SYMBOL(xseg_unregister_type);
EXPORT_SYMBOL(xseg_register_peer);
EXPORT_SYMBOL(xseg_unregister_peer);
EXPORT_SYMBOL(xseg_report_peer_types);
EXPORT_SYMBOL(xseg_enable_driver);
EXPORT_SYMBOL(xseg_disable_driver);
EXPORT_SYMBOL(xseg_create);
EXPORT_SYMBOL(xseg_destroy);
EXPORT_SYMBOL(xseg_join);
EXPORT_SYMBOL(xseg_leave);
EXPORT_SYMBOL(xseg_bind_port);
EXPORT_SYMBOL(xseg_alloc_requests);
EXPORT_SYMBOL(xseg_free_requests);
EXPORT_SYMBOL(xseg_get_request);
EXPORT_SYMBOL(xseg_put_request);
EXPORT_SYMBOL(xseg_prep_request);
EXPORT_SYMBOL(xseg_submit);
EXPORT_SYMBOL(xseg_receive);
EXPORT_SYMBOL(xseg_accept);
EXPORT_SYMBOL(xseg_respond);
EXPORT_SYMBOL(xseg_prepare_wait);
EXPORT_SYMBOL(xseg_cancel_wait);
EXPORT_SYMBOL(xseg_wait_signal);
EXPORT_SYMBOL(xseg_signal);
EXPORT_SYMBOL(xseg_get_port);
EXPORT_SYMBOL(xseg_set_req_data);
EXPORT_SYMBOL(xseg_get_req_data);
EXPORT_SYMBOL(xseg_set_path_next);
EXPORT_SYMBOL(xseg_forward);
EXPORT_SYMBOL(xseg_init_local_signal);
EXPORT_SYMBOL(xseg_quit_local_signal);
EXPORT_SYMBOL(xseg_resize_request);
EXPORT_SYMBOL(xseg_get_objh);
EXPORT_SYMBOL(xseg_put_objh);
EXPORT_SYMBOL(xseg_set_max_requests);
EXPORT_SYMBOL(xseg_get_max_requests);
EXPORT_SYMBOL(xseg_get_allocated_requests);
EXPORT_SYMBOL(xseg_set_freequeue_size);
EXPORT_SYMBOL(xseg_get_data_nonstatic);
EXPORT_SYMBOL(xseg_get_target_nonstatic);
EXPORT_SYMBOL(xseg_get_signal_desc_nonstatic);
EXPORT_SYMBOL(xseg_bind_dynport);
EXPORT_SYMBOL(xseg_leave_dynport);
EXPORT_SYMBOL(xseg_portno_nonstatic);

EXPORT_SYMBOL(__xseg_errbuf);
EXPORT_SYMBOL(__xseg_log);
EXPORT_SYMBOL(init_logctx);
EXPORT_SYMBOL(renew_logctx);
EXPORT_SYMBOL(__xseg_log2);
EXPORT_SYMBOL(xseg_printtrace);
