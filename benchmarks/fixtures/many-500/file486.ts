interface Item486 { id: number; name: string; }
type Key486 = string | number;
export function make486(id: number, name: string): Item486 {
  const value: Item486 = {id, name};
  return value as Item486;
}
export const item486: Item486 = make486(486, "item-486");
