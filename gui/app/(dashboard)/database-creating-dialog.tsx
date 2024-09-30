'use client';

import { Button } from '@/components/ui/button';
import {
  Dialog,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger
} from '@/components/ui/dialog';
import React, { useCallback } from 'react';
import { DatabaseCreatingForm } from './database-creating-form';

export function DatabaseCreatingDialog({ children }: React.PropsWithChildren) {
  const [open, setOpen] = React.useState(false);

  const hide = useCallback(() => {
    setOpen(false);
  }, []);

  return (
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogTrigger asChild>{children}</DialogTrigger>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>Create Database</DialogTitle>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <DatabaseCreatingForm hide={hide}></DatabaseCreatingForm>
        </div>
        <DialogFooter>
          <Button type="submit" form="database-creating">
            Save
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}
