interface Item210 { id: number; name: string; }
type Key210 = string | number;
export function make210(id: number, name: string): Item210 {
  const value: Item210 = {id, name};
  return value as Item210;
}
export const item210: Item210 = make210(210, "item-210");
