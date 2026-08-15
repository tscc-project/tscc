interface Item465 { id: number; name: string; }
type Key465 = string | number;
export function make465(id: number, name: string): Item465 {
  const value: Item465 = {id, name};
  return value as Item465;
}
export const item465: Item465 = make465(465, "item-465");
