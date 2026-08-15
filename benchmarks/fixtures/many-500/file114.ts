interface Item114 { id: number; name: string; }
type Key114 = string | number;
export function make114(id: number, name: string): Item114 {
  const value: Item114 = {id, name};
  return value as Item114;
}
export const item114: Item114 = make114(114, "item-114");
