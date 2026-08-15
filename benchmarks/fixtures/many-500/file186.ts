interface Item186 { id: number; name: string; }
type Key186 = string | number;
export function make186(id: number, name: string): Item186 {
  const value: Item186 = {id, name};
  return value as Item186;
}
export const item186: Item186 = make186(186, "item-186");
