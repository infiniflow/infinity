import {
  Card,
  CardDescription,
  CardHeader,
  CardTitle
} from '@/components/ui/card';
import { ApiUrl } from '@/lib/constant/api';
import { request } from '@/lib/utils';
import { Copy } from 'lucide-react';
import AddIcon from '/public/add.svg';

import { Button } from '@/components/ui/button';
import {
  Dialog,
  DialogClose,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger
} from '@/components/ui/dialog';
import { Input } from '@/components/ui/input';
import React from 'react';

export function DialogCloseButton({ children }: React.PropsWithChildren) {
  return (
    <Dialog>
      <DialogTrigger asChild>{children}</DialogTrigger>
      <DialogContent className="sm:max-w-md">
        <DialogHeader>
          <DialogTitle>Share link</DialogTitle>
          <DialogDescription>
            Anyone who has this link will be able to view this.
          </DialogDescription>
        </DialogHeader>
        <div className="flex items-center space-x-2">
          <div className="grid flex-1 gap-2">
            {/* <Label htmlFor="link" className="sr-only">
              Link
            </Label> */}
            <Input
              id="link"
              defaultValue="https://ui.shadcn.com/docs/installation"
              readOnly
            />
          </div>
          <Button type="submit" size="sm" className="px-3">
            <span className="sr-only">Copy</span>
            <Copy className="h-4 w-4" />
          </Button>
        </div>
        <DialogFooter className="sm:justify-start">
          <DialogClose asChild>
            <Button type="button" variant="secondary">
              Close
            </Button>
          </DialogClose>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}

export default async function HomePage({
  searchParams
}: {
  searchParams: { q: string; offset: string };
}) {
  const ret = await request(ApiUrl.databases);
  console.log('🚀 ~ x:', ret);
  const search = searchParams.q ?? '';
  const offset = searchParams.offset ?? 0;

  return (
    <div className="grid grid-cols-4 gap-4">
      {ret.databases.map((x: string, idx: number) => (
        <Card className="w-full max-w-sm" key={idx}>
          <CardHeader>
            <CardTitle className="text-1xl">{x}</CardTitle>
            <CardDescription>db description</CardDescription>
          </CardHeader>
        </Card>
      ))}
      <Card className="w-full max-w-sm">
        <DialogCloseButton>
          <CardHeader>
            <AddIcon className="w-10 h-10"></AddIcon>
            <CardTitle className="text-1xl">Create Database</CardTitle>
          </CardHeader>
        </DialogCloseButton>
      </Card>
    </div>
  );
}
