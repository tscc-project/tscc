interface Item499 { id: number; name: string; }
type Key499 = string | number;
export function make499(id: number, name: string): Item499 {
  const value: Item499 = {id, name};
  return value as Item499;
}
export const item499: Item499 = make499(499, "item-499");
