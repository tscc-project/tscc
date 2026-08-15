interface Item312 { id: number; name: string; }
type Key312 = string | number;
export function make312(id: number, name: string): Item312 {
  const value: Item312 = {id, name};
  return value as Item312;
}
export const item312: Item312 = make312(312, "item-312");
