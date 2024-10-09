import { Card, CardHeader, CardTitle } from '@/components/ui/card';
import AddIcon from '/public/add.svg';

import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue
} from '@radix-ui/react-select';
import { listDatabase } from '../actions';
import { DatabaseCard } from '../database-card';
import { DatabaseCreatingDialog } from '../database-creating-dialog';

interface IDatabaseSelectProps {
  placeholder?: string;
  options: Array<{ label: string; value: string }>;
}

export function DatabaseSelect({ placeholder, options }: IDatabaseSelectProps) {
  return (
    <Select>
      <SelectTrigger className="w-[180px]">
        <SelectValue placeholder={placeholder} />
      </SelectTrigger>
      <SelectContent>
        {options.map(({ value, label }) => (
          <SelectItem value={value} key={value}>
            {label}
          </SelectItem>
        ))}
      </SelectContent>
    </Select>
  );
}

export default async function HomePage({
  searchParams
}: {
  searchParams: { q: string; offset: string };
}) {
  const ret = await listDatabase();
  const search = searchParams.q ?? '';
  const offset = searchParams.offset ?? 0;

  return (
    <div className="grid grid-cols-4 gap-4">
      {ret.databases.map((x: string, idx: number) => (
        <DatabaseCard key={idx} data={x}></DatabaseCard>
      ))}
      <Card className="w-full max-w-sm">
        <DatabaseCreatingDialog>
          <CardHeader>
            <AddIcon className="w-10 h-10"></AddIcon>
            <CardTitle className="text-1xl">Create Database</CardTitle>
          </CardHeader>
        </DatabaseCreatingDialog>
      </Card>
    </div>
  );
}
