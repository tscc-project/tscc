interface Item475 { id: number; name: string; }
type Key475 = string | number;
export function make475(id: number, name: string): Item475 {
  const value: Item475 = {id, name};
  return value as Item475;
}
export const item475: Item475 = make475(475, "item-475");
