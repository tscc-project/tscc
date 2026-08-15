interface Item289 { id: number; name: string; }
type Key289 = string | number;
export function make289(id: number, name: string): Item289 {
  const value: Item289 = {id, name};
  return value as Item289;
}
export const item289: Item289 = make289(289, "item-289");
