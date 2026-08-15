interface Item135 { id: number; name: string; }
type Key135 = string | number;
export function make135(id: number, name: string): Item135 {
  const value: Item135 = {id, name};
  return value as Item135;
}
export const item135: Item135 = make135(135, "item-135");
