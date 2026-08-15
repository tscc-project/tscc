interface Item111 { id: number; name: string; }
type Key111 = string | number;
export function make111(id: number, name: string): Item111 {
  const value: Item111 = {id, name};
  return value as Item111;
}
export const item111: Item111 = make111(111, "item-111");
