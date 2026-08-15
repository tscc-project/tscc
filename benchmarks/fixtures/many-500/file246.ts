interface Item246 { id: number; name: string; }
type Key246 = string | number;
export function make246(id: number, name: string): Item246 {
  const value: Item246 = {id, name};
  return value as Item246;
}
export const item246: Item246 = make246(246, "item-246");
