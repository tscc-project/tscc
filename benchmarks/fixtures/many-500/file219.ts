interface Item219 { id: number; name: string; }
type Key219 = string | number;
export function make219(id: number, name: string): Item219 {
  const value: Item219 = {id, name};
  return value as Item219;
}
export const item219: Item219 = make219(219, "item-219");
