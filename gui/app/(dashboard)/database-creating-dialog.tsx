import { Button } from '@/components/ui/button';
import {
  Dialog,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger
} from '@/components/ui/dialog';
import React from 'react';
import { DatabaseCreatingForm } from './database-creating-form';

export function DatabaseCreatingDialog({ children }: React.PropsWithChildren) {
  return (
    <Dialog>
      <DialogTrigger asChild>{children}</DialogTrigger>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>Create Database</DialogTitle>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <DatabaseCreatingForm></DatabaseCreatingForm>
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
