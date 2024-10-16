import { Card, CardHeader, CardTitle } from '@/components/ui/card';
import AddIcon from '/public/add.svg';

import { listDatabase } from '../actions';
import { DatabaseCard } from '../database-card';
import { DatabaseCreatingDialog } from '../database-creating-dialog';

export default async function HomePage({}: {
  searchParams: { q: string; offset: string };
}) {
  const ret = await listDatabase();

  return (
    <div className="grid grid-cols-4 gap-4">
      {ret?.databases?.map((x: string, idx: number) => (
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
