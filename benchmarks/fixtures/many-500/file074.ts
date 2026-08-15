interface Item74 { id: number; name: string; }
type Key74 = string | number;
export function make74(id: number, name: string): Item74 {
  const value: Item74 = {id, name};
  return value as Item74;
}
export const item74: Item74 = make74(74, "item-74");
