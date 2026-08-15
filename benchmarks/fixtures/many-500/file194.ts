interface Item194 { id: number; name: string; }
type Key194 = string | number;
export function make194(id: number, name: string): Item194 {
  const value: Item194 = {id, name};
  return value as Item194;
}
export const item194: Item194 = make194(194, "item-194");
