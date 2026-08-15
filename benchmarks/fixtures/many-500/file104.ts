interface Item104 { id: number; name: string; }
type Key104 = string | number;
export function make104(id: number, name: string): Item104 {
  const value: Item104 = {id, name};
  return value as Item104;
}
export const item104: Item104 = make104(104, "item-104");
