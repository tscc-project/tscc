interface Item220 { id: number; name: string; }
type Key220 = string | number;
export function make220(id: number, name: string): Item220 {
  const value: Item220 = {id, name};
  return value as Item220;
}
export const item220: Item220 = make220(220, "item-220");
