import {Button} from '@/components/ui/button';
import {Tabs, TabsContent, TabsList, TabsTrigger} from '@/components/ui/tabs';
import {File, PlusCircle} from 'lucide-react';
import {ProductsTable} from './products-table';
import {DatabaseCard} from "../database-card";
import {Card, CardContent, CardDescription, CardHeader, CardTitle} from "@/components/ui/card";
import {DatabaseCreatingDialog} from "../database-creating-dialog";
import AddIcon from "*.svg";
import {showConfigs, showVariables} from "../actions";
// import { getProducts } from '@/lib/db';

export default async function ProductsPage({
                                               searchParams
                                           }: {
    searchParams: { q: string; offset: string };
}) {
    const search = searchParams.q ?? '';
    const offset = searchParams.offset ?? 0;
    const configs = await showConfigs();
    const variables = await showVariables();
    // const { products, newOffset, totalProducts } = await getProducts(
    //   search,
    //   Number(offset)
    // );
    console.log(variables)
    // const products = await request('http://localhost:3000/products');

    const {newOffset, totalProducts, products} = {
        newOffset: 0,
        totalProducts: 0,
        products: []
    };

    return (
        <div>
            <Card>
                <CardHeader>
                    <CardTitle>Briefs</CardTitle>
                    <CardDescription>Node briefs.</CardDescription>
                </CardHeader>
            </Card>
            <Card>
                <CardHeader>
                    <CardTitle>Configures</CardTitle>
                    <CardDescription>System configures.</CardDescription>
                </CardHeader>
                <CardContent>
                    <p> buffer_manager_size: {configs['buffer_manager_size']} </p>
                    <p> cleanup_interval: {configs['cleanup_interval']} </p>
                    <p> client_port: {configs['client_port']} </p>
                    <p> compact_interval: {configs['compact_interval']} </p>
                    <p> connection_pool_size: {configs['connection_pool_size']} </p>
                    <p> cpu_limit: {configs['cpu_limit']} </p>
                    <p> data_dir: {configs['data_dir']} </p>
                    <p> delta_checkpoint_interval: {configs['delta_checkpoint_interval']} </p>
                    <p> delta_checkpoint_threshold: {configs['delta_checkpoint_threshold']} </p>
                    <p> full_checkpoint_interval: {configs['full_checkpoint_interval']} </p>
                    <p> http_port: {configs['http_port']} </p>
                    <p> log_dir: {configs['log_dir']} </p>
                    <p> log_file_max_size: {configs['log_file_max_size']} </p>
                    <p> log_file_rotate_count: {configs['log_file_rotate_count']} </p>
                    <p> log_filename: {configs['log_filename']} </p>
                    <p> log_level: {configs['log_level']} </p>
                    <p> log_to_stdout: {configs['log_to_stdout']} </p>
                    <p> mem_index_capacity: {configs['mem_index_capacity']} </p>
                    <p> optimize_interval: {configs['optimize_interval']} </p>
                    <p> peer_ip: {configs['peer_ip']} </p>
                    <p> peer_port: {configs['peer_port']} </p>
                    <p> peer_server_connection_pool_size: {configs['peer_server_connection_pool_size']} </p>
                    <p> postgres_port: {configs['postgres_port']} </p>
                    <p> record_running_query: {configs['record_running_query']} </p>
                    <p> resource_dir: {configs['resource_dir']} </p>
                    <p> server_address: {configs['server_address']} </p>
                    <p> temp_dir: {configs['temp_dir']} </p>
                    <p> time_zone: {configs['time_zone']} </p>
                    <p> version: {configs['version']} </p>
                    <p> wal_compact_threshold: {configs['wal_compact_threshold']} </p>
                    <p> wal_dir: {configs['wal_dir']} </p>
                    <p> wal_flush: {configs['wal_flush']} </p>
                </CardContent>
            </Card>
            <Card>
                <CardHeader>
                    <CardTitle>Variables</CardTitle>
                    <CardDescription>System global variables.</CardDescription>
                </CardHeader>
                <CardContent>
                    <p> active_txn_count: {variables['active_txn_count:']} </p>
                    <p> active_wal_filename: {variables['active_wal_filename']} </p>
                    <p> bg_task_count: {variables['bg_task_count']} </p>
                    <p> buffer_object_count: {variables['buffer_object_count']} </p>
                    <p> buffer_usage: {variables['buffer_usage']} </p>
                    <p> cpu_usage: {variables['cpu_usage']} </p>
                    <p> current_timestamp: {variables['current_timestamp']} </p>
                    <p> delta_log_count: {variables['delta_log_count']} </p>
                    <p> jeprof: {variables['jeprof']} </p>
                    <p> next_transaction_id: {variables['next_transaction_id']} </p>
                    <p> open_file_count: {variables['open_file_count']} </p>
                    <p> profile_record_capacity: {variables['profile_record_capacity']} </p>
                    <p> query_count: {variables['query_count']} </p>
                    <p> running_bg_task: {variables['running_bg_task']} </p>
                    <p> running_compact_task: {variables['running_compact_task']} </p>
                    <p> schedule_policy: {variables['schedule_policy']} </p>
                    <p> session_count: {variables['session_count']} </p>
                    <p> system_memory_usage: {variables['system_memory_usage']} </p>
                    <p> total_commit_count: {variables['total_commit_count']} </p>
                    <p> total_rollback_count: {variables['total_rollback_count']} </p>
                    <p> unused_buffer_object: {variables['unused_buffer_object']} </p>
                </CardContent>
            </Card>
        </div>
    );
}
