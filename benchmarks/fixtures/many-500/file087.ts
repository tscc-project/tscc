interface Item87 { id: number; name: string; }
type Key87 = string | number;
export function make87(id: number, name: string): Item87 {
  const value: Item87 = {id, name};
  return value as Item87;
}
export const item87: Item87 = make87(87, "item-87");
